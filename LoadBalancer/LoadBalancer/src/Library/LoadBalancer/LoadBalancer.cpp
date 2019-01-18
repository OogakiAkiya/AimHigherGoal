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
	sendDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//�T�[�o�[�ɑ��M����f�[�^
	recvDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//�T�[�o�[�����M����f�[�^

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
	//��M�����f�[�^���f�[�^���X�g�ɒǉ�����
	clientController->Update(sendDataQueue.get());

	//�T�[�o�[���M�p�f�[�^�̏���
	SendUpdate();

	//�T�[�o�[��M�p����
	RecvUpdate();
}

void LoadBalancer::SendUpdate()
{
	while (1) {
		//�f�[�^�ǂݎ��
		if (sendDataQueue->empty())break;
		NamedPipe::PipeData data = sendDataQueue->front();
		Header header = *(Header*)&data.data[0];				//�w�b�_�[
		std::string userId = std::string(header.playerId);		//�v���C���[ID
		userId.resize(header.playerIdSize);

		if (userMap.count(userId) == 0) {
			bool addProcessFlg = true;
			//�����̃��[�U�[�ɂ��Ȃ��Ȃ烆�[�U�[�̒ǉ�
			std::shared_ptr<NamedPipe>targetPipe = nullptr;
			for (auto pipe : outputPipeMap) {
				int processCount = pipe.second->GetCount();
				if (processCount< ACCESSLIMIT)addProcessFlg=false;
				if (targetPipe == nullptr)targetPipe = pipe.second;
				if (targetPipe->GetCount() > processCount)targetPipe = pipe.second;
			}

			//�v���Z�X�ǉ�����
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
			//�}�b�v�ɓo�^
			userMap.insert(std::make_pair(userId, *targetPipe->GetPipeName()));
			targetPipe->CountUp();
			targetPipe = nullptr;
			

		}
		//playerID����p�C�v�������
		outputPipeMap[userMap[userId]]->Write((char*)data.data, data.byteSize);
		sendDataQueue->pop();
	}
}

void LoadBalancer::RecvUpdate()
{
	//�ꊇ���M
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

		//�S�Ẵ��[�U�[�ɑ���
		clientController->SendAllClient(&recvData);
	}
	*/
}

void LoadBalancer::CreateServerProcess()
{
	//�v���Z�X�쐬
	std::stringstream query;
	int processNumber = process->GetProcessAmount();
	query << "Server.exe " << processNumber;								//�R�}���h���C��MS�쐬
	process->CreateProcessThread(query.str());

	query.str("");
	query.clear(std::stringstream::goodbit);
	
	//���͗p�p�C�v�쐬
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>("LoadBalancer");
	query << INPUTPIPE << processNumber;
	pipe->CreateInputPipe(query.str(),recvDataQueue.get());
	printf("LoadBalancer>>���͗p�p�C�v�쐬:%s\n", query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);

	
	//�o�͗p�̃p�C�v�쐬
	pipe = std::make_shared <NamedPipe>("LoadBalancer");
	query << OUTPUTPIPE << processNumber;
	while (1) {
		if (pipe->CreateClient(query.str())) {
			outputPipeMap.insert(std::make_pair(query.str(), pipe));
			printf("LoadBalancer>>�o�͗p�p�C�v�쐬:%s\n", query.str().c_str());
			break;
		}
	}
}
