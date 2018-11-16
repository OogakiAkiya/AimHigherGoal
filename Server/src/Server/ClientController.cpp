#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
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
	/*スレッド終了処理*/
	thread->detach();

	/*デリート処理*/
	room_numberlist.clear();
	while (1) {
		Client* dele=socketlist[0];
		delete dele;
		MUTEX.Lock();
		socketlist.erase(socketlist.begin());
		MUTEX.Unlock();
		if (socketlist.empty())break;
	}
}

void ClientController::SetSocket(Client* _socket)
{
	socketlist.push_back(_socket);						//ソケットの可変長配列に代入
}

bool ClientController::SerchNumber(int _number)
{
	for (auto element : socketlist) {
		if (element->GetRoomNumber() == _number) {		
			return true;								//同じルームナンバーが見つかった
		}
	}
	return false;
}

void ClientController::StartThread(ClientController* _socket_controller)
{
	thread = new std::thread(ControllerThreadLauncher,(void*)_socket_controller);		//スレッド開始
}


void ClientController::ControllerThread()
{
	while (1) {
		if (socketlist.empty()==false) {
			/*完全データの処理*/
			for (auto client : socketlist) {
				if (client->EmptyCompleteData() == true)break;				//完全データがなければ以下処理は行わない
				MUTEX.Lock();
				std::vector<char>data = *client->GetCompleteData();			//各スレッドの持つ完成したデータを取得
				client->DeleteCompleteData();								//元の配列から取得分のデータを削除
				MUTEX.Unlock();
				DataManipulate(client, &data);								//データの加工処理しそれぞれの処理を行う
			}

			/*切断処理が行われたソケットを削除*/
			int count = 0;
			for (auto client : socketlist) {
				if (client->GetState() == -1) {
					Client* deletesock;
					deletesock = socketlist[count];
					MUTEX.Lock();
					socketlist.erase(socketlist.begin() + count);
					MUTEX.Unlock();
					delete deletesock;
					break;
				}
				count++;
			}

		}
	}
}


void ClientController::DataManipulate(Client* _socket, std::vector<char>* _data)
{
	char id = *(char*)&_data->at(0);
	switch (id) {

	/*座標更新*/
	case 0x15: {
		char encode[BYTESIZE];																		//暗号化データを入れる
		char senddata[BYTESIZE];																		//送信データ


		//printf("\n受け取ったデータ\nid=0x15\n");
		/*データの整形をし値をセット*/
		float recvdata = *(float*)&_data->at(sizeof(char));
		_socket->GetData()->SetX(recvdata);
		//printf("x=%f\n", recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 1);
		_socket->GetData()->SetY(recvdata);
		//printf("y=%f\n", recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 2);
		_socket->GetData()->SetZ(recvdata);
		//printf("z=%f\n", recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 3);
		_socket->GetData()->SetAngle(recvdata);
		//printf("angle=%f\n", recvdata);
		int recvdata_int = *(int*)&_data->at(sizeof(char) + sizeof(float) * 4);
		_socket->GetData()->SetAnimation(recvdata_int);
		//printf("animation=%d\n", recvdata_int);

		/*送信データの作成*/
		PosData data;
		data.size = sizeof(PosData) - sizeof(int);
		data.id = 0x16;
		data.x = _socket->GetData()->GetX();
		data.y = _socket->GetData()->GetY();
		data.z = _socket->GetData()->GetZ();
		data.angle = _socket->GetData()->GetAngle();
		data.animation = _socket->GetData()->GetAnimation();

		/*暗号化処理*/
		char* origin = (char*)&data;
		int encode_size = CIPHER.GetOpenSSLAES()->Encode(encode, origin, sizeof(PosData));		//暗号化
		memcpy(senddata, &encode_size, sizeof(int));
		memcpy(&senddata[sizeof(int)], encode, encode_size);

		/*送信処理*/
		send(_socket->GetSocket(), senddata,sizeof(int)+encode_size, 0);						//作成したデータの作成
		//printf("送信データ\nsize=%d\nid=%04x\nx=%f\ny=%f\nz=%f\nangle=%f\nanimation=%d\n", data.size, data.id, data.x, data.y, data.z, data.angle, data.animation);
		break;
	}

	/*攻撃処理*/
	case 0x17:
		char encode[BYTESIZE];																				//暗号データ
		char senddata[BYTESIZE];																				//送信データ

		/*今は使ってないが複数のPCとつなげた場合ここを使用することになる*/
		int recvdata = *(int*)&_data->at(sizeof(char));													//誰が攻撃されたのかがこの中に入っている

		/*送信データの作成*/
		BaseData sendbuf;
		sendbuf.size = sizeof(BaseData) - sizeof(int);
		sendbuf.id = 0x18;

		/*暗号化処理*/
		int encode_size = CIPHER.GetOpenSSLAES()->Encode(encode, (char*)&sendbuf, sizeof(BaseData));	//暗号化処理
		memcpy(senddata, &encode_size, sizeof(int));
		memcpy(&senddata[sizeof(int)], encode, encode_size);
		
		/*送信処理*/
		send(_socket->GetSocket(), (char*)&senddata,encode_size+sizeof(int), 0);				//作成したデータの作成
		break;
	}
}


