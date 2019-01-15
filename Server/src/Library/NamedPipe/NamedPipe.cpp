#include"../../Include.h"
#include"../Mutex/ExtensionMutex.h"
#include"NamedPipe.h"

void InputPipeThread(std::string _pipeName, std::queue<NamedPipe::PipeData>* _dataList);


NamedPipe::NamedPipe() {
}

NamedPipe::~NamedPipe()
{
	if (pipeHandle == INVALID_HANDLE_VALUE) return;
	if (deleteFlg) {
		FlushFileBuffers(pipeHandle);						//キャッシュ内にデータがあった場合それをディスクに書き込む
	}
	DisconnectNamedPipe(pipeHandle);						//名前付きパイプ切断
	CloseHandle(pipeHandle);								//名前付きパイプを閉じる
}

bool NamedPipe::CreateServer(std::string _pipeName)
{
	std::stringstream query;
	query << "\\\\.\\pipe\\" << _pipeName;
	//名前付きパイプの作成
	pipeHandle = CreateNamedPipe(query.str().c_str(), //コマンドライン
		PIPE_ACCESS_INBOUND,						  // オープンモード
		PIPE_TYPE_BYTE | PIPE_WAIT,					  // パイプモード
		1,											  // 最大インスタンス数
		0,											  // 出力ブッファサイズ
		0,											  // 入力バッファサイズ
		NULL,										  // タイムアウト時間(NULLだとタイムアウト無し)
		NULL										  // lpSecurityAttributes
	);

	if (pipeHandle == INVALID_HANDLE_VALUE) {
		printf("Create ServerPipe Error\n");
		return false;
	}
	deleteFlg = true;
	return true;
}

bool NamedPipe::CreateClient(std::string _pipeName)
{
	std::stringstream query;
	query << "\\\\.\\pipe\\" << _pipeName;

	pipeHandle = CreateFile(query.str().c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (pipeHandle == INVALID_HANDLE_VALUE) {
		printf("Create  ClientPipe Error\n");
		return false;
	}
	return true;
}


bool NamedPipe::ConnectRecv()
{
	if (!ConnectNamedPipe(pipeHandle, NULL)) {
		printf("ConnectPipe Error\n");
		CloseHandle(pipeHandle);
		return false;
	}
	printf("接続が来ました\n");
	return true;
}

int NamedPipe::Read(char * _data,int _dataLength)
{
	DWORD dwBytesRead;
	if (!ReadFile(pipeHandle, _data, _dataLength, &dwBytesRead, NULL)) {
		printf("ReadFile Error\n");
		return 0;
	}
	return dwBytesRead;
}

int NamedPipe::Write(char * _data, int _dataLengeh)
{
	DWORD writeSize;
	if (!WriteFile(pipeHandle, _data, _dataLengeh, &writeSize, NULL)) {
		printf("Write Error");
		return 0;
	}
	return writeSize;
}

void NamedPipe::CreateInputPipe(std::string _pipeName,std::queue<PipeData>* _dataList)
{
	//Readのスレッドを作成する
	std::thread thread(InputPipeThread, _pipeName, _dataList);
	thread.detach();
}

void InputPipeThread(std::string _pipeName, std::queue<NamedPipe::PipeData>* _dataList) {
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

		
		//データの追加
		NamedPipe::PipeData addData;
		addData.byteSize = size;
		memcpy(&addData.data[0], buf, size);
		MUTEX.Lock();
		_dataList->push(addData);
		MUTEX.Unlock();
	}
	pipe = nullptr;
}
