#include"../Include.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Mutex/ExtensionMutex.h"
#include"../Library/Data/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"../Library/Socket/Socket.h"
#include"../Library/CurlWrapper/CurlWrapper.h"
#include"Client.h"
#include "ClientController.h"
#include "Server.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

Server::Server(int _processNumber)
{
	//�C���X�^���X�̐���
	Cipher::GetInstance();
	recvDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//���[�h�o�����T�[�ɑ��M����f�[�^
	clientController = std::make_unique<ClientController>();
	outputPipe = std::make_unique<NamedPipe>();

	//�����_�������̏�����
	srand(time(0));

	//���o�̓p�C�v�쐬
	CreatePipe(_processNumber);
}


Server::~Server()
{
	Cipher::DeleteInstance();
	clientController = nullptr;
	while (1) {
		if (recvDataQueue->empty())break;
		recvDataQueue->pop();
	}
	recvDataQueue = nullptr;

	char buf[128]="EXIT";
	outputPipe->Write(buf, strlen(buf));
	outputPipe = nullptr;
}

void Server::Update()
{
	while (1) {
		MUTEX.Lock();
		if (recvDataQueue->empty()) {
			MUTEX.Unlock();
			break;
		}

		NamedPipe::PipeData data = recvDataQueue->front();
		recvDataQueue->pop();
		MUTEX.Unlock();

		int dataSize = *(int*)&data.data[0];					//�S�̂̃o�C�g�T�C�Y
		int idSize = *(int*)&data.data[sizeof(int)];			//ID�T�C�Y
		//playreID�̎擾
		char* temp = (char*)malloc(idSize);
		memcpy(temp, &data.data[sizeof(int) * 2], idSize);
		std::shared_ptr<std::string> playerId = std::make_shared<std::string>(temp);
		playerId->resize(idSize);																	//id�̃T�C�Y�����ɂȂ�Ȃ��̂�resize

		//id����v����N���C�A���g�̌���&�ǉ�
		if (clientMap.count(playerId->c_str()) == 0) {											//�d�����Ȃ�����
			std::shared_ptr<Client> client = std::make_shared<Client>();
			//ID�̒ǉ�
			client->GetData()->SetId(playerId);
			clientMap.insert(std::make_pair(playerId->c_str(), client));
			client = nullptr;
		}
		//data�̒ǉ�
		clientMap[playerId->c_str()]->AddData(&data);
	}

	//client����
	for (auto itr = clientMap.begin(); itr != clientMap.end(); ++itr) {
		itr->second->Update();
	}
}

void Server::CreatePipe(int _processNumber)
{
	std::stringstream query;

	//���͗p�p�C�v�쐬
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>();
	query << INPUTPIPE << _processNumber;
	pipe->CreateInputPipe(query.str(), recvDataQueue.get());
	printf("���͗p�p�C�v�쐬:%s", query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);


	//�o�͗p�̃p�C�v�쐬
	query << OUTPUTPIPE << _processNumber;
	while (1) {
		if (outputPipe->CreateClient(query.str())) {
			printf("�o�͗p�p�C�v�쐬:%s", query.str().c_str());
			break;
		}
	}


}
