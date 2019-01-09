#include"../Include.h"
#include"../Library/Data/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"../Library/CurlWrapper/CurlWrapper.h"
#include"../Library/Socket/Socket.h"
#include"Client.h"
#include "ClientController.h"


ClientController::ClientController()
{
}


ClientController::~ClientController()
{
	//解放処理
	roomNumberList.clear();
	for (int i = 0; i < socketList.size(); i++) {
		socketList[i]=nullptr;
	}
	socketList.clear();
}

void PosRegistration(std::string _data)
{
	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(nullptr, "http://lifestyle-qa.com/update_user_arraydata.php", _data);
	curl = nullptr;
}

void ClientController::Update()
{
	ControllerUpdate();
	SocketUpdate();
	CreateDBData();
}

void ClientController::SetSocket(std::shared_ptr<Client> _socket)
{
	socketList.push_back(_socket);
}


void ClientController::ControllerUpdate()
{
	//サーバーに接続しているクライアントがいるか判定
	if (socketList.empty() == true)return;
	//完全データの処理
	for (auto& client : socketList) {
		if (client->EmptyCompleteData() == true)break;				//完全データがなければ以下処理は行わない
		DataManipulate(client.get(), client->GetCompleteData());
		client->DeleteCompleteData();
	}

	//切断処理が行われたソケットを削除
	int count = 0;
	for (int i = 0; i < socketList.size(); i++) {
		if (socketList[i]->GetState() == -1) {
			socketList[i] == nullptr;
			socketList.erase(socketList.begin() + i);
		}
	}
}

void ClientController::SocketUpdate()
{
	if (socketList.empty() == true)return;
	for (auto& client : socketList) {
		client->Update();
	}
}


void ClientController::DataManipulate(Client* _socket, std::vector<char>* _data)
{
	char id = *(char*)&_data->at(0);
	switch (id) {
	case 0x01: {
		//送信用データ
		char recvData[BYTESIZE];
		char sendData[BYTESIZE];																	//送信データ

		//プレイヤーIDの整形
		int idsize = *(int*)&_data->at(sizeof(char));
		char* temp=(char*)malloc(idsize);
		memcpy(temp, &_data->at(sizeof(char) + sizeof(int)), idsize);
		std::shared_ptr<std::string> playerId = std::make_shared<std::string>(temp);
		playerId->resize(idsize);																	//idのサイズが一定にならないのでresize

		_socket->GetData()->SetId(playerId);

		//プレイヤーの座標取得
		_socket->GetCurl()->DBGetPos(recvData,playerId);
		_socket->GetData()->SetX(*(float*)recvData);
		_socket->GetData()->SetY(*(float*)&recvData[sizeof(float)]);
		_socket->GetData()->SetZ(*(float*)&recvData[sizeof(float) * 2]);

		//送信データ作成
		UserData userData;
		userData.data.size = sizeof(UserData) - sizeof(int);
		userData.data.id = 0x02;
		userData.x = _socket->GetData()->GetX();
		userData.y = _socket->GetData()->GetY();
		userData.z = _socket->GetData()->GetZ();


		//送信用データの作成
		int amountSize=CreateSendData(sendData, _socket, (char*)&userData, sizeof(UserData));

		//送信処理
		send(_socket->GetSocket(), sendData, amountSize, 0);

		//解放処理
		free(temp);
		playerId = nullptr;
		_socket->SetPosGetFlg();
		break;
	}
	//座標更新
	case 0x15: {
		//送信用データ
		char sendData[BYTESIZE];																	//送信データ

		//データの整形をし値をセット
		_socket->GetData()->SetX(*(float*)&_data->at(sizeof(char)));
		_socket->GetData()->SetY(*(float*)&_data->at(sizeof(char) + sizeof(float) * 1));
		_socket->GetData()->SetZ(*(float*)&_data->at(sizeof(char) + sizeof(float) * 2));
		_socket->GetData()->SetAngle(*(float*)&_data->at(sizeof(char) + sizeof(float) * 3));
		_socket->GetData()->SetAnimation(*(int*)&_data->at(sizeof(char) + sizeof(float) * 4));

		//送信データの作成
		PosData data;
		data.size = sizeof(PosData) - sizeof(int);
		data.id = 0x16;
		data.x = _socket->GetData()->GetX();
		data.y = _socket->GetData()->GetY();
		data.z = _socket->GetData()->GetZ();
		data.angle = _socket->GetData()->GetAngle();
		data.animation = _socket->GetData()->GetAnimation();

		//送信用データの作成
		int amountSize = CreateSendData(sendData, _socket, (char*)&data, sizeof(PosData));

		//送信処理
		send(_socket->GetSocket(), sendData, amountSize , 0);
		//printf("送信データ\nsize=%d\nid=%04x\nx=%f\ny=%f\nz=%f\nangle=%f\nanimation=%d\n", data.size, data.id, data.x, data.y, data.z, data.angle, data.animation);
		break;
	}

	//攻撃処理
	case 0x17:
		char sendData[BYTESIZE];																				//送信データ

		//今は使ってないが複数のPCとつなげた場合ここを使用することになる
		int recvData = *(int*)&_data->at(sizeof(char));													//誰が攻撃されたのかがこの中に入っている

		//送信データの作成
		BaseData sendBuf;
		sendBuf.size = sizeof(BaseData) - sizeof(int);
		sendBuf.id = 0x18;

		//送信用データの作成
		int amountSize = CreateSendData(sendData, _socket, (char*)&sendBuf, sizeof(BaseData));

		//送信処理
		send(_socket->GetSocket(), (char*)&sendData,amountSize, 0);
		break;
	}
}

void ClientController::CreateDBData()
{
	std::stringstream query;						//webサーバーに送るデータ
	std::string output;								//queryのままだとエラーが起こりstring型に入れるとなくなる
	std::vector<std::shared_ptr<Data>> datas;		//送信するデータ一覧

	//送るデータの選別
	for (auto socket : socketList) {
		if (socket->GetPosGetFlg()) {
			datas.push_back(socket->GetData());
		}
	}
	if (datas.size() <= 0)return;

	//データの作成
	query << "amount=" << datas.size();
	for (int i = 0; i < datas.size(); i++) {
		query << "&" << "player" << i << "=" << datas[i]->GetId()->c_str();
		query << "&" << "x" << i << "=" << datas[i]->GetX();
		query << "&" << "y" << i << "=" << datas[i]->GetY();
		query << "&" << "z" << i << "=" << datas[i]->GetZ();
	}
	query >> output;

	std::thread thread(PosRegistration, output);
	thread.detach();


}


int ClientController::CreateSendData(char* _encryptionData, Client* _socket,char* _originalData,int _dataLen)
{
	char encode[BYTESIZE];																		//暗号化データを入れる
	int encodeSize = _socket->GetAES()->Encode(encode, _originalData, _dataLen);		//暗号化
	memcpy(_encryptionData, &encodeSize, sizeof(int));											//元のデータサイズが先頭に入る
	memcpy(&_encryptionData[sizeof(int)], encode, encodeSize);

	return encodeSize+sizeof(int);
}

