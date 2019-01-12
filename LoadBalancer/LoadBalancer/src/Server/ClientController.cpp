#include"../Include.h"
#include"../Library/Data/Data.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Socket/Socket.h"
#include"Client.h"
#include "ClientController.h"

#pragma comment(lib,"Ws2_32.lib")

ClientController::ClientController()
{
	//ソケット初期化
	socket = std::make_shared<Socket>();
	socket = socket->
		SetProtocolVersion_IPv4().								//IPv4
		SetProtocol_TCP().										//TCP
		SetIpAddress("0,0,0,0").								//アドレス指定
		SetPortNumber(PORTNUMBER).								//ポート番号
		SetAsynchronous().										//非同期化
		ServerCreate(socket);									//サーバーソケット生成

}


ClientController::~ClientController()
{
	//解放処理
	for (int i = 0; i < socketList.size(); i++) {
		socketList[i]=nullptr;
	}
	socketList.clear();
	socket = nullptr;
}

void ClientController::Update(std::queue<NamedPipe::PipeData>* _dataList)
{
	AcceptSocket();
	ControllerUpdate();
	SocketUpdate();
	GetPipeData(_dataList);
}

void ClientController::ControllerUpdate()
{
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

void ClientController::GetPipeData(std::queue<NamedPipe::PipeData>* _dataList)
{
	//サーバーに接続しているクライアントがいるか判定
	if (socketList.empty() == true)return;
	//パイプデータの処理
	for (auto& client : socketList) {
		if (client->EmptyPipeData() == true)break;				//完全データがなければ以下処理は行わない
		NamedPipe::PipeData pipeData = *client->GetPipeData();
		_dataList->push(pipeData);
		//データの削除
		client->DeletePipeData();
	}

}


void ClientController::AcceptSocket()
{
	std::shared_ptr<Client> clientSocket;											//クライアントのソケット情報を一時的に保存する変数

	//accept
	SOCKET initSocket;
	initSocket = INVALID_SOCKET;													//client_socket初期化
	initSocket = socket->Accept();
	if (initSocket == INVALID_SOCKET)return;										//アクセスがなかった場合ここで終わる

	//クライアントのソケットをコントローラークラスに追加する
	clientSocket = std::make_shared<Client>();
	clientSocket->GetData()->SetSocket(initSocket);									//接続のあったソケットをセット
	socketList.push_back(clientSocket);
	printf("アクセスがありました\n");
}

