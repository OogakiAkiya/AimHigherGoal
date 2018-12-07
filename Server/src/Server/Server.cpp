#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
#include"../Library/Cipher/OpenSSLRSA.h"
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
	//シングルトン作成
	ExtensionMutex::GetInstance();								//mutex管理classインスタンス生成
	Cipher::GetInstance();
	
	//ランダム処理の初期化
	srand(time(0));

	//ソケット初期化
	socket = Socket::Instantiate()->
		SetProtocolVersion_IPv4().								//IPv6
		SetProtocol_TCP().										//TCP
		SetIpAddress("0.0.0.0").
		SetPortNumber("49155").									//ポート番号
		ServerCreate();											//サーバーソケット

	if (socket == nullptr) delete this;
	clientController=std::make_shared<ClientController>();
	//ソケットの管理クラスのスレッド開始
	clientController->StartThread(clientController.get());
	
}


Server::~Server()
{
	socket->Close();
	ExtensionMutex::DeleteInstance();													//mutex管理classインスタンスの削除
	Cipher::DeleteInstance();
	if(socket!=nullptr)delete socket;
	clientController = nullptr;
}

void Server::AcceptLoop()
{
	while (1) {
		AcceptSocket();
	}
}


void Server::AcceptSocket()
{
	std::shared_ptr<Client> clientSocket;																//クライアントのソケット情報を一時的に保存する変数

	//accept
	SOCKET initSocket;
	initSocket = INVALID_SOCKET;					//client_socket初期化
	initSocket = socket->Accept();

		//クライアントのソケットをコントローラークラスに追加する
		clientSocket = std::make_shared<Client>();
		clientSocket->SetSocket(initSocket);											//Socketクラスにクライアントの情報を代入する
		MUTEX.Lock();																	//排他制御Lock
		clientController->SetSocket(clientSocket);										//作ったクライアント情報はSocketControllerクラスで管理
		MUTEX.Unlock();																	//排他制御Unlock

		printf("来ました\n");
		clientSocket->StartRecvThread(clientSocket.get());									//recv処理のスレッド開始
}
