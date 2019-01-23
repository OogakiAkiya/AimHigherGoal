#include"../Include.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Mutex/ExtensionMutex.h"
#include"../Library/Data/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"../Library/Socket/Socket.h"
#include"../Library/CurlWrapper/CurlWrapper.h"
#include"Client.h"
#include "Server.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

Server::Server(int _processNumber)
{
	processNumber = _processNumber;
	std::stringstream discriminationName;
	discriminationName << "Server" << _processNumber;

	//インスタンスの生成
	Cipher::GetInstance();
	recvDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//ロードバランサーに送信するデータ
	outputPipe = std::make_unique<NamedPipe>(discriminationName.str());

	//ランダム処理の初期化
	srand(time(0));

	//入出力パイプ作成
	CreatePipe(_processNumber);
}


Server::~Server()
{
	Cipher::DeleteInstance();
	while (1) {
		if (recvDataQueue->empty())break;
		recvDataQueue->pop();
	}
	recvDataQueue = nullptr;

	char buf[128]="EXIT";
	outputPipe->Write(buf, strlen(buf));
	outputPipe = nullptr;
}

void Server::Update()
{
	//InputPipeにきているデータの管理
	InputPipeProcessing();

	//client更新処理
	ClientUpdate();

	//データベースにクライアントの情報を登録する
	DBRegistration();
}

void Server::CreatePipe(int _processNumber)
{
	std::stringstream query;
	std::stringstream discriminationName;
	discriminationName<< "Server" << _processNumber;

	//入力用パイプ作成
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>(discriminationName.str());
	query << INPUTPIPE << _processNumber;
	pipe->CreateInputPipe(query.str(), recvDataQueue.get());
	printf("Server%d>>入力用パイプ作成:%s\n",_processNumber, query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);


	//出力用のパイプ作成
	query << OUTPUTPIPE << _processNumber;
	while (1) {
		if (outputPipe->CreateClient(query.str())) {
			printf("Server%d>>出力用パイプ作成:%s\n",_processNumber,query.str().c_str());
			break;
		}
	}


}

void Server::ClientUpdate()
{
	for (auto client : clientMap) {
		client.second->Update();
		while (1) {
			if (client.second->GetSendData()->empty())break;
			outputPipe->Write((char*)&client.second->GetSendData()->front().data, client.second->GetSendData()->front().byteSize);
			client.second->GetSendData()->pop();
		}
	}
}

void Server::InputPipeProcessing()
{
	while (1) {
		MUTEX.Lock();
		if (recvDataQueue->empty()) {
			MUTEX.Unlock();
			break;
		}

		NamedPipe::PipeData data = recvDataQueue->front();					//パイプから来たデータの取得
		recvDataQueue->pop();
		MUTEX.Unlock();

		//playreIDの取得
		int idSize = *(int*)&data.data[sizeof(int)];						//IDサイズ
		std::shared_ptr<std::string> playerId = std::make_shared<std::string>(&data.data[sizeof(int)*2]);
		playerId->resize(idSize);

		//idが一致するクライアントの検索&追加
		if (clientMap.count(playerId->c_str()) == 0) {
			//クライアントの追加
			std::shared_ptr<Client> client = std::make_shared<Client>();
			client->GetData()->SetId(playerId);
			clientMap.insert(std::make_pair(playerId->c_str(), client));
			client = nullptr;
		}

		//dataの追加
		clientMap[playerId->c_str()]->AddData(&data);
	}

}

void PosRegistration(std::shared_ptr<std::string> _data)
{
	std::shared_ptr<std::string> data = _data;
	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(nullptr, "http://lifestyle-qa.com/update_user_arraydata.php", *data.get());
	curl = nullptr;
	data = nullptr;
}


void Server::DBRegistration()
{
	query = make_shared<std::string>();

	std::stringstream tempQuery;						//webサーバーに送るデータ
	std::vector<std::shared_ptr<Data>> datas;			//送信するデータ一覧

	//送るデータの選別
	for (auto client : clientMap) {
		if (client.second->GetPosGetFlg()) {
			datas.push_back(client.second->GetData());
		}
	}

	if (datas.size() <= 0)return;

	//データの作成
	tempQuery << "amount=" << datas.size();
	for (int i = 0; i < datas.size(); i++) {
		tempQuery << "&" << "player" << i << "=" << datas[i]->GetId()->c_str();
		tempQuery << "&" << "x" << i << "=" << datas[i]->GetX();
		tempQuery << "&" << "y" << i << "=" << datas[i]->GetY();
		tempQuery << "&" << "z" << i << "=" << datas[i]->GetZ();
	}
	tempQuery >> *query;

	std::thread thread(PosRegistration,query);
	thread.detach();
	query = nullptr;
}

