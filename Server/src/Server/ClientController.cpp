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
	SocketUpdate();
	DBCreateData();
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
