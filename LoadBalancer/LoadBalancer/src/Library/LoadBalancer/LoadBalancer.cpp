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

	//CreateServerProcess();


	std::stringstream query;
	//入力用パイプ作成
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>();
	query << INPUTPIPE << 0;
	pipe->CreateInputPipe(query.str(),recvDataQueue.get());
	printf("入力用パイプ作成:%s", query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);


	//出力用のパイプ作成
	pipe = std::make_shared <NamedPipe>();
	query << OUTPUTPIPE << 0;
	while (1) {
		if (pipe->CreateClient(query.str())) {
			outputPipeMap.insert(std::make_pair(query.str(), pipe));
			printf("出力用パイプ作成:%s", query.str().c_str());
			break;
		}
	}
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
	clientController->Update(sendDataQueue.get());			//受信したデータをデータリストに追加する

	//サーバー送信用データの処理
	while (1) {
		if (sendDataQueue->empty())break;
		NamedPipe::PipeData data=sendDataQueue->front();
		Header header = *(Header*)&data.data[0];				//ヘッダー
		std::string userId =std::string(header.playerId);		//プレイヤーID
		userId.resize(header.playerIdSize);

		if (userMap.count(userId)==0) {
			//ユーザーの追加
			std::shared_ptr<NamedPipe>targetPipe=nullptr;
			for (auto pipe : outputPipeMap) {
				if (targetPipe == nullptr)targetPipe = pipe.second;
				if(targetPipe->GetCount()>pipe.second->GetCount())targetPipe = pipe.second;
			}
			userMap.insert(std::make_pair(userId, *targetPipe->GetPipeName()));
			targetPipe->CountUp();
			targetPipe = nullptr;
		}
		//playerIDからパイプ名を特定
		outputPipeMap[userMap[userId]]->Write((char*)data.data, data.byteSize);
		sendDataQueue->pop();

	}

	//サーバー受信用処理
	while (1) {
		MUTEX.Lock();
		if (recvDataQueue->empty()) {
			MUTEX.Unlock();
			break;
		}
		NamedPipe::PipeData recvData = recvDataQueue->front();
		recvDataQueue->pop();
		MUTEX.Unlock();
		clientController->SendAllClient(&recvData);
	}
}

void LoadBalancer::CreateServerProcess()
{
	//プロセス作成
	std::stringstream query;
	int processNumber = process->GetProcessAmount();
	query << "Server.exe " << processNumber;								//コマンドラインMS作成
	process->CreateProcessThread(query.str());
	Sleep(500);

	query.str("");
	query.clear(std::stringstream::goodbit);
	
	//入力用パイプ作成
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>();
	query << INPUTPIPE << processNumber;
	pipe->CreateInputPipe(query.str(),recvDataQueue.get());
	printf("入力用パイプ作成:%s", query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);

	
	//出力用のパイプ作成
	pipe = std::make_shared <NamedPipe>();
	query << OUTPUTPIPE << processNumber;
	while (1) {
		if (pipe->CreateClient(query.str())) {
			outputPipeMap.insert(std::make_pair(query.str(), pipe));
			printf("出力用パイプ作成:%s", query.str().c_str());
			break;
		}
	}
}
