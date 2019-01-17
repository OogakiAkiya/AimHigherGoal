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
#include "Server.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

Server::Server(int _processNumber)
{
	processNumber = _processNumber;
	std::stringstream discriminationName;
	discriminationName << "Server" << _processNumber;
	//�C���X�^���X�̐���
	Cipher::GetInstance();
	recvDataQueue = std::make_unique<std::queue<NamedPipe::PipeData>>();								//���[�h�o�����T�[�ɑ��M����f�[�^
	outputPipe = std::make_unique<NamedPipe>(discriminationName.str());

	//�����_�������̏�����
	srand(time(0));

	//���o�̓p�C�v�쐬
	CreatePipe(_processNumber);
}


Server::~Server()
{
	Cipher::DeleteInstance();
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
	//InputPipe�ɂ��Ă���f�[�^�̊Ǘ�
	InputPipeProcessing();

	//client�X�V����
	ClientUpdate();

	//�f�[�^�x�[�X�ɃN���C�A���g�̏���o�^����
	DBRegistration();
}

void Server::CreatePipe(int _processNumber)
{
	std::stringstream query;
	std::stringstream discriminationName;
	discriminationName<< "Server" << _processNumber;

	//���͗p�p�C�v�쐬
	std::shared_ptr<NamedPipe> pipe = std::make_shared <NamedPipe>(discriminationName.str());
	query << INPUTPIPE << _processNumber;
	pipe->CreateInputPipe(query.str(), recvDataQueue.get());
	printf("Server%d>>���͗p�p�C�v�쐬:%s\n",_processNumber, query.str().c_str());
	pipe = nullptr;

	query.str("");
	query.clear(std::stringstream::goodbit);


	//�o�͗p�̃p�C�v�쐬
	query << OUTPUTPIPE << _processNumber;
	while (1) {
		if (outputPipe->CreateClient(query.str())) {
			printf("Server%d>>�o�͗p�p�C�v�쐬:%s\n",_processNumber,query.str().c_str());
			break;
		}
	}


}

void Server::ClientUpdate()
{
	for (auto client : clientMap) {
		client.second->Update();
		while (1) {
			if (client.second->GetSendData()->empty())break;
			outputPipe->Write((char*)&client.second->GetSendData()->front().data, client.second->GetSendData()->front().byteSize);
			client.second->GetSendData()->pop();
		}
	}
}

void Server::InputPipeProcessing()
{
	while (1) {
		MUTEX.Lock();
		if (recvDataQueue->empty()) {
			MUTEX.Unlock();
			break;
		}

		NamedPipe::PipeData data = recvDataQueue->front();					//�p�C�v���痈���f�[�^�̎擾
		recvDataQueue->pop();
		MUTEX.Unlock();

		//playreID�̎擾
		int idSize = *(int*)&data.data[sizeof(int)];						//ID�T�C�Y
		char* temp = new char[idSize];
		memcpy(temp, &data.data[sizeof(int) * 2], idSize);
		std::shared_ptr<std::string> playerId = std::make_shared<std::string>(temp);
		delete temp;
		playerId->resize(idSize);											//id�̃T�C�Y�����ɂȂ�Ȃ��̂�resize

		//id����v����N���C�A���g�̌���&�ǉ�
		if (clientMap.count(playerId->c_str()) == 0) {
			//�N���C�A���g�̒ǉ�
			std::shared_ptr<Client> client = std::make_shared<Client>();
			client->GetData()->SetId(playerId);
			clientMap.insert(std::make_pair(playerId->c_str(), client));
			client = nullptr;
		}
		//data�̒ǉ�
		clientMap[playerId->c_str()]->AddData(&data);
	}

}

void PosRegistration(std::string _data)
{
	std::unique_ptr<CurlWrapper> curl = std::make_unique<CurlWrapper>();
	curl->HTTPConnect(nullptr, "http://lifestyle-qa.com/update_user_arraydata.php", _data, "DBCreateData");
	curl = nullptr;
}


void Server::DBRegistration()
{
	std::stringstream query;						//web�T�[�o�[�ɑ���f�[�^
	std::string output;								//query�̂܂܂��ƃG���[���N����string�^�ɓ����ƂȂ��Ȃ�
	std::vector<std::shared_ptr<Data>> datas;		//���M����f�[�^�ꗗ

	//����f�[�^�̑I��
	for (auto client : clientMap) {
		if (client.second->GetPosGetFlg()) {
			datas.push_back(client.second->GetData());
		}
	}

	if (datas.size() <= 0)return;

	//�f�[�^�̍쐬
	query << "amount=" << datas.size();
	for (int i = 0; i < datas.size(); i++) {
		query << "&" << "player" << i << "=" << datas[i]->GetId()->c_str();
		query << "&" << "x" << i << "=" << datas[i]->GetX();
		query << "&" << "y" << i << "=" << datas[i]->GetY();
		query << "&" << "z" << i << "=" << datas[i]->GetZ();
	}
	query >> output;

	std::thread thread(PosRegistration, output);
	thread.detach();

}

