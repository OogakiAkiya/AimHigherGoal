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
	//‰ğ•úˆ—
	roomNumberList.clear();
	for (int i = 0; i < clientList.size(); i++) {
		clientList[i]=nullptr;
	}
	clientList.clear();
}


void ClientController::Update()
{
	SocketUpdate();
	//DBCreateData();
}

void ClientController::AddClient(std::shared_ptr<Client> _client)
{
	clientList.push_back(_client);
}

void ClientController::SocketUpdate()
{
	if (clientList.empty() == true)return;
	for (auto& client : clientList) {
		client->Update();
	}
}

