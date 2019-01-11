#include"../../Include.h"
#include"../Mutex/ExtensionMutex.h"
#include"../Process/Process.h"
#include"../NamedPipe/NamedPipe.h"
#include"LoadBalancer.h"

void InputPipeThread(std::string _pipeName, std::vector<OutputData>* _dataList);

LoadBalancer::LoadBalancer()
{
	process = std::make_unique<Process>();
}

LoadBalancer::~LoadBalancer()
{
	process = nullptr;
	for (auto itr = inputPipeMap.begin(); itr != inputPipeMap.end(); ++itr) {
		itr->second = nullptr;
	}
	inputPipeMap.clear();
}

void LoadBalancer::Updata()
{
	CreateServerProcess();
	Sleep(10000);
	CreateOutputPipe(0);
}

void LoadBalancer::CreateServerProcess()
{
	//プロセス作成
	std::stringstream query;
	int processNumber = process->GetProcessAmount();
	query << "Server.exe " << processNumber;								//コマンドラインMS作成
	process->CreateProcessThread(query.str());
	Sleep(500);
	CreateInputPipe(processNumber);
}

void LoadBalancer::CreateInputPipe(int _pipeNumber)
{
	//インプットの作成
	std::stringstream query;
	query << INPUTPIPE << _pipeNumber;
	//Readのスレッドを作成する
	std::thread thread(InputPipeThread,query.str(),&dataList);
	thread.detach();

	//stringstreamのバッファ削除
	query.str("");
	query.clear(std::stringstream::goodbit);

}

void LoadBalancer::CreateOutputPipe(int _pipeNumber)
{
	std::stringstream query;
	std::unique_ptr<NamedPipe> pipe = std::make_unique <NamedPipe>();

	query << OUTPUTPIPE << _pipeNumber;
	while (1) {
		pipe = std::make_unique <NamedPipe>();
		if (pipe->CreateClient(query.str())) {
			break;
		}
		pipe = nullptr;
	}

	char szBuff[255] = "aaa";
	if (pipe->Write(szBuff, strlen(szBuff)) == 0) {
		return;
	}

	Sleep(1000);
	char Buff[255] = "EXIT";
	if (pipe->Write(Buff, strlen(Buff)) == 0) {
		return;
	}

	pipe = nullptr;

}

void InputPipeThread(std::string _pipeName,std::vector<OutputData>* _dataList) {
	std::unique_ptr<NamedPipe> pipe = std::make_unique <NamedPipe>();
	if (!pipe->CreateServer(_pipeName)) {
		pipe = nullptr;
		return;
	}
	if (!pipe->ConnectRecv()) {
		pipe = nullptr;
		return;
	}
	while (1) {
		char buf[256];
		int size = pipe->Read(buf, sizeof(buf));
		//スレッド終了処理
		if (!strncmp("EXIT", buf, 4)) { // Exit Check
			printf("スレッド終了");
			break;
		}
		buf[size] = '\0';
		printf("Server : %s", buf);
		Sleep(1);

		/*
		//データの追加
		OutputData addData;
		addData.byteSize = size;
		memcpy(&addData.data[0], buf, size);
		MUTEX.Lock();
		_dataList->push_back(addData);
		MUTEX.Unlock();
		*/
	}
}
