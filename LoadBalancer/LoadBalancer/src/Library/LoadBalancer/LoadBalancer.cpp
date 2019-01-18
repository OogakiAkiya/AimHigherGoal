#include"../../Include.h"
#include"../Data/Data.h"
#include"../Mutex/ExtensionMutex.h"
#include"../Process/Process.h"
#include"../NamedPipe/NamedPipe.h"
#include"../Socket/Socket.h"
#include"../../Server/Client.h"
#include"../../Server/ClientController.h"
#include"LoadBalancer.h"


LoadBalancer::LoadBalancer()
{
	process = std::make_unique<Process>();
	clientController = std::make_unique<ClientController>();
	sendDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//サーバーに送信するデータ
	recvDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//サーバーから受信するデータ

	CreateServerProcess();
}

LoadBalancer::~LoadBalancer()
{

	process = nullptr;
	clientController = nullptr;
	while (1) {
		if (sendDataQueue->empty())break;
		sendDataQueue->pop();
	}
	sendDataQueue = nullptr;
	while (1) {
		if (recvDataQueue->empty())break;
		recvDataQueue->pop();
	}
	recvDataQueue = nullptr;

	for (auto& pipe : outputPipeMap) {
		pipe.second = nullptr;
	}
	outputPipeMap.clear();
}

void LoadBalancer::Updata()
{
	//受信したデータをデータリストに追加する
	clientController->Update(sendDataQueue.get());

	//サーバー送信用データの処理
	SendUpdate();

	//サーバー受信用処理
	RecvUpdate();
}

void LoadBalancer::SendUpdate()
{
	while (1) {
		//データ読み取り
		if (sendDataQueue->empty())break;
		NamedPipe::PipeData data = sendDataQueue->front();
		Header header = *(Header*)&data.data[0];				//ヘッダー
		std::string userId = std::string(header.playerId);		//プレイヤーID
		userId.resize(header.playerIdSize);

		if (userMap.count(userId) == 0) {
			bool addProcessFlg = true;
			//既存のユーザーにいないならユーザーの追加
			std::shared_ptr<NamedPipe>targetPipe = nullptr;
			for (auto pipe : outputPipeMap) {
				int processCount = pipe.second->GetCount();
				if (processCount< ACCESSLIMIT)addProcessFlg=false;
				if (targetPipe == nullptr)targetPipe = pipe.second;
				if (targetPipe->GetCount() > processCount)targetPipe = pipe.second;
			}

			//プロセス追加処理
			if (addProcessFlg == true) {
				CreateServerProcess();
				std::string string;
				for (auto pipe : outputPipeMap) {
					if (targetPipe->GetCount() > pipe.second->GetCount()) {
						targetPipe = pipe.second;
					}
					string = *targetPipe->GetPipeName();
				}
				printf("\n%s\n", string.c_str());

			}
			//マップに登録
			userMap.insert(std::make_pair(userId, *targetPipe->GetPipeName()));
			targetPipe->CountUp();
			targetPipe = nullptr;
			

		}
		//playerIDからパイプ名を特定
		outputPipeMap[userMap[userId]]->Write((char*)data.data, data.byteSize);
		sendDataQueue->pop();
	}
}

void LoadBalancer::RecvUpdate()
{
	//一括送信
	std::vector<char> sendData;
	MUTEX.Lock();
	for (int i = 0, nowSize = 0; i < recvDataQueue->size(); i++) {
		sendData.resize(nowSize+recvDataQueue->front().byteSize);
		memcpy(&sendData[nowSize], &recvDataQueue->front().data[0], recvDataQueue->front().byteSize);
		nowSize += recvDataQueue->front().byteSize;
		recvDataQueue->pop();
	}
	MUTEX.Unlock();

	clientController->SendAllClient(&sendData[0], sendData.size());


	/*
	while (1) {
		MUTEX.Lock();
		if (recvDataQueue->empty()) {
			MUTEX.Unlock();
			break;
		}
		NamedPipe::PipeData recvData = recvDataQueue->front();
		recvDataQueue->pop();
		MUTEX.Unlock();

		//全てのユーザーに送る
		clientController->SendAllClient(&recvData);
	}
	*/
}

void LoadBalancer::CreateServerProcess()
{
	//プロセス作成
	std::stringstream query;
	int processNumber = process->GetProcessAmount();
	query << "Server.exe " << processNumber;								//コマンドラインMS作成
	process->CreateProcessThread(query.str());

	query.str("");
	query.clear(std::stringstream::goodbit);
	
	//入力用パイプ作成
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>("LoadBalancer");
	query << INPUTPIPE << processNumber;
	pipe->CreateInputPipe(query.str(),recvDataQueue.get());
	printf("LoadBalancer>>入力用パイプ作成:%s\n", query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);

	
	//出力用のパイプ作成
	pipe = std::make_shared <NamedPipe>("LoadBalancer");
	query << OUTPUTPIPE << processNumber;
	while (1) {
		if (pipe->CreateClient(query.str())) {
			outputPipeMap.insert(std::make_pair(query.str(), pipe));
			printf("LoadBalancer>>出力用パイプ作成:%s\n", query.str().c_str());
			break;
		}
	}
}
