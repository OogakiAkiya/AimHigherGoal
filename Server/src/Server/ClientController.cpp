#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"CurlWrapper.h"
#include"Socket.h"
#include"Client.h"
#include "ClientController.h"


ClientController::ClientController()
{
}


ClientController::~ClientController()
{
	//スレッド終了処理
	thread->detach();

	//解放処理
	roomNumberList.clear();
	while (1) {
		Client* dele=socketList[0];
		delete dele;
		MUTEX.Lock();
		socketList.erase(socketList.begin());
		MUTEX.Unlock();
		if (socketList.empty())break;
	}
}

void ClientController::SetSocket(Client* _socket)
{
	socketList.push_back(_socket);						//ソケットの可変長配列に代入
}

bool ClientController::SerchNumber(int _number)
{
	for (auto element : socketList) {
		if (element->GetRoomNumber() == _number) {		
			return true;								//同じルームナンバーが見つかった
		}
	}
	return false;
}

void ClientController::StartThread(ClientController* _socketController)
{
	thread = new std::thread(ControllerThreadLauncher,(void*)_socketController);		//スレッド開始
}


void ClientController::ControllerThread()
{
	while (1) {
		//サーバーに接続しているクライアントがいるか判定
		if (socketList.empty() == true)continue;

		//完全データの処理
		for (auto client : socketList) {
			if (client->EmptyCompleteData() == true)break;				//完全データがなければ以下処理は行わない
			MUTEX.Lock();
			std::vector<char>data = *client->GetCompleteData();			//各スレッドの持つ完成したデータを取得
			client->DeleteCompleteData();								//元の配列から取得分のデータを削除
			MUTEX.Unlock();
			DataManipulate(client, &data);								//データの加工処理しそれぞれの処理を行う
		}

		//切断処理が行われたソケットを削除
		int count = 0;
		for (auto client : socketList) {
			if (client->GetState() == -1) {
				Client* deletesock;
				deletesock = socketList[count];
				MUTEX.Lock();
				socketList.erase(socketList.begin() + count);
				MUTEX.Unlock();
				delete deletesock;
				break;
			}
			count++;
		}
	}
}


void ClientController::DataManipulate(Client* _socket, std::vector<char>* _data)
{
	char id = *(char*)&_data->at(0);
	switch (id) {
	case 0x01: {
		//送信用データ
		char recvData[BYTESIZE];
		char encode[BYTESIZE];																		//暗号化データを入れる
		char sendData[BYTESIZE];																	//送信データ

		int idsize = *(int*)&_data->at(sizeof(char));
		char* temp=(char*)malloc(idsize);
		memcpy(temp, &_data->at(sizeof(char) + sizeof(int)), idsize);
		std::string id(temp);
		MUTEX.Lock();
		_socket->GetData()->SetId(id);
		MUTEX.Unlock();

		//プレイヤーの座標取得
		_socket->GetCurl()->DBGetPos(recvData,id);
		float x = *(float*)recvData;
		float y = *(float*)&recvData[sizeof(float)];
		float z = *(float*)&recvData[sizeof(float)*2];

		UserData userData;
		userData.data.size = sizeof(UserData) - sizeof(int);
		userData.data.id = 0x02;
		userData.x = x;
		userData.y = y;
		userData.z = z;

		//暗号化処理
		char* origin = (char*)&userData;
		int encodeSize = _socket->GetAES()->Encode(encode, origin, sizeof(UserData));		//暗号化

		memcpy(sendData, &encodeSize, sizeof(int));
		memcpy(&sendData[sizeof(int)], encode, encodeSize);

		//送信処理
		send(_socket->GetSocket(), sendData, sizeof(int) + encodeSize, 0);						//作成したデータの作成

		_socket->StartHttpThread();
		free(temp);
		break;
	}
	//座標更新
	case 0x15: {
		//送信用データ
		char encode[BYTESIZE];																		//暗号化データを入れる
		char sendData[BYTESIZE];																	//送信データ

		//データの整形をし値をセット
		MUTEX.Lock();
		_socket->GetData()->SetX(*(float*)&_data->at(sizeof(char)));
		_socket->GetData()->SetY(*(float*)&_data->at(sizeof(char) + sizeof(float) * 1));
		_socket->GetData()->SetZ(*(float*)&_data->at(sizeof(char) + sizeof(float) * 2));
		_socket->GetData()->SetAngle(*(float*)&_data->at(sizeof(char) + sizeof(float) * 3));
		_socket->GetData()->SetAnimation(*(int*)&_data->at(sizeof(char) + sizeof(float) * 4));
		MUTEX.Unlock();

		//送信データの作成
		PosData data;
		data.size = sizeof(PosData) - sizeof(int);
		data.id = 0x16;
		data.x = _socket->GetData()->GetX();
		data.y = _socket->GetData()->GetY();
		data.z = _socket->GetData()->GetZ();
		data.angle = _socket->GetData()->GetAngle();
		data.animation = _socket->GetData()->GetAnimation();

		//暗号化処理
		char* origin = (char*)&data;
		int encodeSize = _socket->GetAES()->Encode(encode, origin, sizeof(PosData));		//暗号化

		memcpy(sendData, &encodeSize, sizeof(int));
		memcpy(&sendData[sizeof(int)], encode, encodeSize);

		//送信処理
		send(_socket->GetSocket(), sendData,sizeof(int)+encodeSize, 0);						//作成したデータの作成
		//printf("送信データ\nsize=%d\nid=%04x\nx=%f\ny=%f\nz=%f\nangle=%f\nanimation=%d\n", data.size, data.id, data.x, data.y, data.z, data.angle, data.animation);
		break;
	}

	//攻撃処理
	case 0x17:
		char encode[BYTESIZE];																				//暗号データ
		char sendData[BYTESIZE];																				//送信データ

		//今は使ってないが複数のPCとつなげた場合ここを使用することになる
		int recvData = *(int*)&_data->at(sizeof(char));													//誰が攻撃されたのかがこの中に入っている

		//送信データの作成
		BaseData sendBuf;
		sendBuf.size = sizeof(BaseData) - sizeof(int);
		sendBuf.id = 0x18;

		//暗号化処理
		int encodeSize = _socket->GetAES()->Encode(encode, (char*)&sendBuf, sizeof(BaseData));	//暗号化処理
		memcpy(sendData, &encodeSize, sizeof(int));
		memcpy(&sendData[sizeof(int)], encode, encodeSize);
		
		//送信処理
		send(_socket->GetSocket(), (char*)&sendData,encodeSize+sizeof(int), 0);				//作成したデータの作成
		break;
	}
}


