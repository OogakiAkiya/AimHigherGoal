#include"../Include.h"
#include"../Library/Mutex/ExtensionMutex.h"
#include"../Library/NamedPipe/NamedPipe.h"
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
	for (int i = 0; i < clientList.size(); i++) {
		clientList[i]=nullptr;
	}
	clientList.clear();
}

void PosRegistration(std::string _data)
{
	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(nullptr, "http://lifestyle-qa.com/update_user_arraydata.php", _data,"DBCreateData");
	curl = nullptr;
}

void ClientController::Update()
{
	ControllerUpdate();
	SocketUpdate();
	DBCreateData();
}

void ClientController::AddClient(std::shared_ptr<Client> _client)
{
	clientList.push_back(_client);
}


void ClientController::ControllerUpdate()
{
	//サーバーに接続しているクライアントがいるか判定
	if (clientList.empty() == true)return;
	//完全データの処理
	for (auto& client : clientList) {
		if (client->EmptyCompleteData() == true)break;				//完全データがなければ以下処理は行わない
		DataManipulate(client.get(), client->GetCompleteData());
		client->DeleteCompleteData();
	}

	//切断処理が行われたソケットを削除
	int count = 0;
	for (int i = 0; i < clientList.size(); i++) {
		if (clientList[i]->GetState() == -1) {
			clientList[i] == nullptr;
			clientList.erase(clientList.begin() + i);
		}
	}
}

void ClientController::SocketUpdate()
{
	if (clientList.empty() == true)return;
	for (auto& client : clientList) {
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
		DBGetPos(recvData, playerId);

		//取得したデータをセット
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

void ClientController::DBCreateData()
{
	std::stringstream query;						//webサーバーに送るデータ
	std::string output;								//queryのままだとエラーが起こりstring型に入れるとなくなる
	std::vector<std::shared_ptr<Data>> datas;		//送信するデータ一覧

	//送るデータの選別
	for (auto socket : clientList) {
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

void ClientController::DBGetPos(char * _data, std::shared_ptr<std::string> _userId)
{
	//ユーザー追加処理
	std::stringstream query;
	std::string output = "";																//送信用データ
	std::string buf;																		//受け取ったデータを格納する
	std::string error;
	char recvdata[256];

	//送信データの生成
	query << "player=" << _userId.get()->c_str();
	query >> output;

	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(&buf, "http://lifestyle-qa.com/get_pos.php", output.c_str(),*_userId.get());
	curl = nullptr;

	//jsonから値の取得
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (buf.length() != 0) {
		auto json = json11::Json::parse(buf, error);

		x = std::stof(json["x"].string_value());
		y = std::stof(json["y"].string_value());
		z = std::stof(json["z"].string_value());
	}

	//データ代入
	memcpy(recvdata, &x, sizeof(float));
	memcpy(&recvdata[sizeof(float)], &y, sizeof(float));
	memcpy(&recvdata[sizeof(float) * 2], &z, sizeof(float));
	memcpy(_data, recvdata, sizeof(float) * 3);

}


int ClientController::CreateSendData(char* _encryptionData, Client* _socket,char* _originalData,int _dataLen)
{
	char encode[BYTESIZE];																		//暗号化データ

	int encodeSize = _socket->GetAES()->Encode(encode, _originalData, _dataLen);				//暗号化
	memcpy(_encryptionData, &encodeSize, sizeof(int));											//元のデータサイズが先頭に入る
	memcpy(&_encryptionData[sizeof(int)], encode, encodeSize);									//暗号化データを入れる

	return encodeSize+sizeof(int);																//暗号化のデータサイズ+先頭に入れたデータサイズ
}

