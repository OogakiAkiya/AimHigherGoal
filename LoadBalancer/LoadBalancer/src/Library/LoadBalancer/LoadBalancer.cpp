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
	for (auto itr = outputPipeMap.begin(); itr != outputPipeMap.end(); ++itr) {
		itr->second = nullptr;
	}
	outputPipeMap.clear();
	//namePipeList.erase(namePipeList.begin());
}

void LoadBalancer::Updata()
{
	CreateServerProcess();
}

void LoadBalancer::CreateServerProcess()
{
	//プロセス作成
	std::stringstream query;
	int processNumber = process->GetProcessAmount();
	query << "Server.exe " << processNumber;								//コマンドラインMS作成
	process->CreateProcessThread(query.str());
	Sleep(500);
	CreateServerPipe(processNumber);
}

void LoadBalancer::CreateServerPipe(int _pipeNumber)
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

	//アウトプットの作成
	/*
	query << SERVEROUTPUT << _pipeNumber;
	if (!pipe->CreateServer(query.str())) {
		pipe = nullptr;
		return;
	}
	if (!pipe->ConnectRecv()) {
		pipe = nullptr;
		return;
	}

	outputPipeMap.insert(std::make_pair(query.str(), pipe));					//接続したタイミングでパイプを登録
	*/
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
