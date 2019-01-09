#include"../Include.h"
#include"../Library/Mutex/ExtensionMutex.h"
#include"../Library/Data/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"../Library/Socket/Socket.h"
#include"../Library/CurlWrapper/CurlWrapper.h"
#include"Client.h"
#include "ClientController.h"
#include "Server.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

Server::Server()
{
	//シングルトン作成
	Cipher::GetInstance();
	
	//ランダム処理の初期化
	srand(time(0));

	//ソケット初期化
	socket = make_shared<Socket>();
	socket = socket->
		SetProtocolVersion_IPv4().								//IPv4
		SetProtocol_TCP().										//TCP
		SetIpAddress("0,0,0,0").								//アドレス指定
		SetPortNumber("49155").									//ポート番号
		SetAsynchronous().										//非同期化
		ServerCreate(socket);									//サーバーソケット生成
	clientController=std::make_shared<ClientController>();
	
}


Server::~Server()
{
	Cipher::DeleteInstance();
	socket = nullptr;
	clientController = nullptr;
}

void Server::AcceptLoop()
{
	while (1) {
		if (socket == nullptr)return;
		AcceptSocket();
		clientController->Update();
	}
}


void Server::AcceptSocket()
{
	std::shared_ptr<Client> clientSocket;																//クライアントのソケット情報を一時的に保存する変数

	//accept
	SOCKET initSocket;
	initSocket = INVALID_SOCKET;					//client_socket初期化
	initSocket = socket->Accept();
	if (initSocket == INVALID_SOCKET)return;
	//クライアントのソケットをコントローラークラスに追加する
	clientSocket = std::make_shared<Client>();
	clientSocket->SetSocket(initSocket);											//Socketクラスにクライアントの情報を代入する
	clientController->SetSocket(clientSocket);										//作ったクライアント情報はSocketControllerクラスで管理

	printf("来ました\n");
}
