#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
#include"../Library/Cipher/Cipher.h"
#include"Socket.h"
#include"CurlWrapper.h"
#include"Client.h"
#include "ClientController.h"
#include "Server.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

Server::Server()
{
	//シングルトン作成*/
	ExtensionMutex::GetInstance();								//mutex管理classインスタンス生成
	Cipher::GetInstance();
	/*ランダム処理の初期化*/
	srand(time(0));

	/*ソケット初期化*/
	m_socket = Socket::Instantiate()->
		SetProtocolVersion_IPv6().								//IPv6
		SetProtocol_TCP().										//TCP
		SetIpAddress("0.0.0.0").
		SetPortNumber("49155").									//ポート番号
		ServerCreate();											//サーバーソケット

	/*ソケットの管理クラスのスレッド開始*/
	socket_controller.StartThread(&socket_controller);

}


Server::~Server()
{
	m_socket->Close();
	ExtensionMutex::DeleteInstance();													//mutex管理classインスタンスの削除
	Cipher::DeleteInstance();
	delete m_socket;
}

void Server::AcceptLoop()
{
	while (1) {
		AcceptSocket();
	}
}


void Server::AcceptSocket()
{
	Client* client_socket;																//クライアントのソケット情報を一時的に保存する変数

	/*accept*/
	SOCKET init_socket;
	init_socket = INVALID_SOCKET;					//client_socket初期化
	init_socket = m_socket->Accept();

		/*クライアントのソケットをコントローラークラスに追加する*/
		client_socket = new Client();
		client_socket->SetSocket(init_socket);											//Socketクラスにクライアントの情報を代入する
		MUTEX.Lock();																	//排他制御Lock
		socket_controller.SetSocket(client_socket);										//作ったクライアント情報はSocketControllerクラスで管理
		MUTEX.Unlock();																	//排他制御Unlock

		printf("来ました\n");
		client_socket->StartRecvThread(client_socket);										//recv処理のスレッド開始
		client_socket->StartHttpThread();												//Http通信のスレッド開始
}


//void ClientRoutine(Socket _socket) { ; }
//{
//	SOCKET client=(SOCKET)ptr;			//受け取ったクライアント情報を保存
//	char receivbuf[512];					//受け取るバッファサイズを[]の中に指定
//	int iResult;
//	for (auto element : roomlist) {
//		if (element.SerchSocket(client) == true) {
//			element.RoomMatchUpdate(client);
//		}
//
//	}
//
//	
//	while (1) 
//	{
//		/*受信*/
//		iResult= recv(client/*送信元のソケット*/, receivbuf/*データ*/, 512/*データのバッファサイズ*/, 0);			//データ受け取り
//		if (iResult > 0) {
//			switch (receivbuf[0]) {
//			case 0x01: {										//ルームを立てる
//				int room_num;												//ルーム番号
//				while (1) {
//					bool breakflg=true;
//					room_num = (int)floor(rand() % 900000 + 100000);			//6桁の番号を作成
//					for (auto element : roomlist) {
//						if (element.GetRoomNumber() == room_num)breakflg = false;
//					}
//					if (breakflg == true)break;
//				}
//				Room* newroom = new Room();					//ルーム作成		
//				newroom->SetRoomNumber(room_num);
//				newroom->SetMasterSocket((SOCKET)client);
//				roomlist.push_back(*newroom);
//				delete newroom;
//				break;
//			}
//
//			case 0x03: {															//ルームに参加
//				bool serchflg = false;												//
//				int received_num = CharToNum(&receivbuf[1], 3);						//受け取った値を10進数に変換
//				for (auto element : roomlist) {										//ルームナンバーの探索
//					if (element.GetRoomNumber() == received_num) {
//						element.SetSocket((SOCKET)client);							//ソケットをRoomクラスにセット(Roomに空きがあれば追加、空きがなければないと通知)
//						serchflg = true;
//						break;
//					}
//				}
//				if (serchflg == false) {
//					//ルームがない
//					char secondbuf[3];
//					secondbuf[0] = 0x05;
//					secondbuf[1] = 0x02;
//					secondbuf[2] = 0xff;
//					iResult = send(client, secondbuf, 3, 0);
//				}
//				break;
//			}
//			case 0x06:									//マッチングキャンセル
//				//for (auto element : roomlist) {
//					//element.DeleteSocket((SOCKET)ptr);
//					break;
//				//}
//			}
//
//			/*送信*/
//			iResult = send(client, receivbuf, 512, 0);
//
//		}else if (iResult == 0) {								//信号が送られてこなかったとき
//			printf("切断されました\n");
//			for (auto element : roomlist) {
//				if (element.SerchSocket(client) == true) {
//					element.DeleteSocket((SOCKET)client);
//					break;
//				}
//			}
//			//切断したソケットの削除
//			auto itr = find(sockets.begin(), sockets.end(), client);							//ソケットを探す
//			sockets.erase(remove(sockets.begin(), sockets.end(), distance(sockets.begin(), itr)), sockets.end());			//上で探したものを削除
//			break;
//		}
//		else {														//それ以外の時
//			printf("recv failed:%d\n", WSAGetLastError());
//			break;
//		}
//		
//	}
//}
/*
// bufからbytes分を読み込んでintで返す
int CharToNum(char *buf, int bytes) {
	int ret = 0;
	int p = 0;
	while (p < bytes) {
		ret += (ret * 0xff) + (unsigned char)buf[p++];
	}
	return ret;
}

// numをbytes分だけbufにバイナリで書き込む
void NumToChar(int num, int bytes, char *buf) {
	int p = 0;
	while (p < bytes) {
		buf[bytes - (p++) - 1] = (num % 0x100) & 0xFF;
		num /= 0x100;
	}
}
*/