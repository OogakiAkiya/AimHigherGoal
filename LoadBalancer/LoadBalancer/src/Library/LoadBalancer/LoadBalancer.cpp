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
	sendDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//�N���C�A���g�ɑ��M����f�[�^
	CreateServerProcess();

	/*
	//���͗p�̃p�C�v�쐬
	std::stringstream processName;
	processName << INPUTPIPE << 0;
	inputPipe->CreateInputPipe(processName.str(), sendDataQueue.get());
	printf("���͗p�p�C�v�쐬:%s", processName.str().c_str());
	*/
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
	for (auto itr = outputPipeMap.begin(); itr != outputPipeMap.end(); ++itr) {
		itr->second = nullptr;
	}
	outputPipeMap.clear();
}

void LoadBalancer::Updata()
{
	clientController->Update(sendDataQueue.get());			//��M�����f�[�^���f�[�^���X�g�ɒǉ�����
	//�f�[�^�����ɓ��������ǂ����̊m�F�p
	if (!sendDataQueue->empty()) {
		NamedPipe::PipeData data;
		data=sendDataQueue->front();
	}
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
	//�v���Z�X�쐬
	std::stringstream query;
	int processNumber = process->GetProcessAmount();
	query << "Server.exe " << processNumber;								//�R�}���h���C��MS�쐬
	process->CreateProcessThread(query.str());
	Sleep(500);

	query.str("");
	query.clear(std::stringstream::goodbit);
	
	//���͗p�p�C�v�쐬
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>();
	query << INPUTPIPE << processNumber;
	pipe->CreateInputPipe(query.str(),sendDataQueue.get());
	printf("���͗p�p�C�v�쐬:%s", query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);

	
	//�o�͗p�̃p�C�v�쐬
	pipe = std::make_shared <NamedPipe>();
	query << OUTPUTPIPE << processNumber;
	while (1) {
		if (pipe->CreateClient(query.str())) {
			outputPipeMap.insert(std::make_pair(query.str(), pipe));
			printf("�o�͗p�p�C�v�쐬:%s", query.str().c_str());
			break;
		}
	}
}
