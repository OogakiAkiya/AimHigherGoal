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
	dataList = std::make_unique<std::queue<NamedPipe::PipeData>>();								//クライアントに送信するデータ
}

LoadBalancer::~LoadBalancer()
{

	process = nullptr;
	clientController = nullptr;
	while (1) {
		if (dataList->empty())break;
		dataList->pop();
	}
	dataList = nullptr;
	for (auto itr = inputPipeMap.begin(); itr != inputPipeMap.end(); ++itr) {
		itr->second = nullptr;
	}
	inputPipeMap.clear();
}

void LoadBalancer::Updata()
{
	clientController->Update(dataList.get());
	if (!dataList->empty()) {
		NamedPipe::PipeData data;
		data=dataList->front();
		printf("%d", data.byteSize);
	}
	/*
	CreateServerProcess();
	Sleep(10000);

	std::stringstream query;
	query << OUTPUTPIPE << 0;


	Sleep(1000);
	if (true) {
		std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>();
		char szBuff[255] = "aaa";
		if (pipe->CreateClient(query.str())) {
			inputPipeMap.insert(std::make_pair(query.str(), pipe));
			pipe->Write(szBuff, strlen(szBuff));
			printf("\naaa成功\n");
		}
		pipe = nullptr;

	}
	if (true) {
		char Buff[255] = "EXIT";

		inputPipeMap[query.str()]->Write(Buff, strlen(Buff));
		printf("\nEXIT成功\n");
		inputPipeMap[query.str()] = nullptr;

	}
	*/

}

void LoadBalancer::Temp()
{
	/*
	for (auto data : dataList) {
		printf("%s\n", data.data);
	}
	Sleep(2000);
	*/
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

	std::unique_ptr<NamedPipe> pipe = std::make_unique <NamedPipe>();
	//インプットの作成
	query << INPUTPIPE << 0;

	pipe->CreateInputPipe(query.str(),dataList.get());
	pipe = nullptr;
}
