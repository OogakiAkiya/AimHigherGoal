#include"../../Include.h"
#include"NamedPipe.h"

NamedPipe::NamedPipe() {
}

NamedPipe::~NamedPipe()
{
	if (pipeHandle == INVALID_HANDLE_VALUE) return;
	if (deleteFlg) {
		FlushFileBuffers(pipeHandle);						//キャッシュ内にデータがあった場合それをディスクに書き込む
		DisconnectNamedPipe(pipeHandle);						//名前付きパイプ切断
	}
	CloseHandle(pipeHandle);								//名前付きパイプを閉じる
}

bool NamedPipe::CreateServer(std::string _pipeName)
{
	std::stringstream query;
	query << "\\\\.\\pipe\\" << _pipeName;
	//名前付きパイプの作成
	pipeHandle = CreateNamedPipe(query.str().c_str(), //lpName
		PIPE_ACCESS_INBOUND,            // dwOpenMode
		PIPE_TYPE_BYTE | PIPE_WAIT,     // dwPipeMode
		1,                              // nMaxInstances
		0,                              // nOutBufferSize
		0,                              // nInBufferSize
		NULL,                            // nDefaultTimeOut
		NULL
	);                          // lpSecurityAttributes

	if (pipeHandle == INVALID_HANDLE_VALUE) {
		printf("Create Error\n");
		Sleep(1000);
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
		printf("Create Error\n");
		return false;
	}
	return true;
}


bool NamedPipe::ConnectRecv()
{
	if (!ConnectNamedPipe(pipeHandle, NULL)) {
		printf("ConnectError\n");
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
