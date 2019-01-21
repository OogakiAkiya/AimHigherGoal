#include"../../Include.h"
#include"../Mutex/ExtensionMutex.h"
#include"NamedPipe.h"

void InputPipeThread(std::string _discriminationName,std::string _pipeName, std::queue<NamedPipe::PipeData>* _dataList);


NamedPipe::NamedPipe()
{
}

NamedPipe::NamedPipe(std::string _discriminationName)
{
	discriminationName = _discriminationName;
}

NamedPipe::~NamedPipe()
{
	if (pipeHandle == INVALID_HANDLE_VALUE) return;
	if (deleteFlg) {
		FlushFileBuffers(pipeHandle);						//�L���b�V�����Ƀf�[�^���������ꍇ������f�B�X�N�ɏ�������
	}
	DisconnectNamedPipe(pipeHandle);						//���O�t���p�C�v�ؒf
	CloseHandle(pipeHandle);								//���O�t���p�C�v�����
}

bool NamedPipe::CreateServer(std::string _pipeName)
{
	pipeName = _pipeName;
	std::stringstream query;
	query << "\\\\.\\pipe\\" << _pipeName;
	//���O�t���p�C�v�̍쐬
	pipeHandle = CreateNamedPipe(query.str().c_str(), //�R�}���h���C��
		PIPE_ACCESS_INBOUND,						  // �I�[�v�����[�h
		PIPE_TYPE_BYTE | PIPE_WAIT,					  // �p�C�v���[�h
		1,											  // �ő�C���X�^���X��
		0,											  // �o�̓u�b�t�@�T�C�Y
		0,											  // ���̓o�b�t�@�T�C�Y
		NULL,										  // �^�C���A�E�g����(NULL���ƃ^�C���A�E�g����)
		NULL										  // lpSecurityAttributes
	);

	if (pipeHandle == INVALID_HANDLE_VALUE) {
		printf("%s>>Create ServerPipe Error\n",discriminationName.c_str());
		return false;
	}
	deleteFlg = true;
	return true;
}

bool NamedPipe::CreateClient(std::string _pipeName)
{
	pipeName = _pipeName;
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
		printf("%s>>Create  ClientPipe Error\n",discriminationName.c_str());
		return false;
	}
	return true;
}


bool NamedPipe::ConnectRecv()
{
	if (!ConnectNamedPipe(pipeHandle, NULL)) {
		printf("%s>>ConnectPipe Error\n",discriminationName.c_str());
		CloseHandle(pipeHandle);
		return false;
	}
	printf("%s>>Connect Recv Pipe\n", discriminationName.c_str());
	return true;
}

int NamedPipe::Read(char * _data,int _dataLength)
{
	DWORD dwBytesRead;
	if (!ReadFile(pipeHandle, _data, _dataLength, &dwBytesRead, NULL)) {
		printf("%s>>ReadFile Error\n", discriminationName.c_str());
		return 0;
	}
	return dwBytesRead;
}

int NamedPipe::Write(char * _data, int _dataLengeh)
{
	DWORD writeSize;
	if (!WriteFile(pipeHandle, _data, _dataLengeh, &writeSize, NULL)) {
		printf("%s>>Write Error", discriminationName.c_str());
		return 0;
	}
	return writeSize;
}

void NamedPipe::CreateInputPipe(std::string _pipeName,std::queue<PipeData>* _dataList)
{
	//Read�̃X���b�h���쐬����
	std::thread thread(InputPipeThread,discriminationName, _pipeName, _dataList);
	thread.detach();
}

void NamedPipe::CountUp()
{
	count++;
}

void NamedPipe::CountDown()
{
	count--;
	if (count < 0)count = 0;
}

int NamedPipe::GetCount()
{
	return count;
}

std::string* NamedPipe::GetPipeName()
{
	return &pipeName;
}

void InputPipeThread(std::string _discriminationName,std::string _pipeName, std::queue<NamedPipe::PipeData>* _dataList) {
	std::unique_ptr<NamedPipe> pipe = std::make_unique <NamedPipe>(_discriminationName);
	if (!pipe->CreateServer(_pipeName)) {
		pipe = nullptr;
		return;
	}
	if (!pipe->ConnectRecv()) {
		pipe = nullptr;
		return;
	}
	while (1) {
		char buf[NamedPipe::RECVBYTESIZE];
		int size = pipe->Read(buf, NamedPipe::RECVBYTESIZE);
		//�X���b�h�I������
		if (!strncmp("EXIT", buf, 4)) { // Exit Check
			printf("%s>>InputPipeThread Finish\n",_discriminationName.c_str());
			break;
		}
		
		//�f�[�^�̒ǉ�
		NamedPipe::PipeData addData;
		addData.byteSize = size;
		memcpy(&addData.data[0], buf, size);
		MUTEX.Lock();
		_dataList->push(addData);
		MUTEX.Unlock();
	}
	pipe = nullptr;
}
