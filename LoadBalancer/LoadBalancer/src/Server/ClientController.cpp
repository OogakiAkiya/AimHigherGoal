#include"../Include.h"
#include"../Library/Mutex/ExtensionMutex.h"
#include"../Library/Data/Data.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Socket/Socket.h"
#include"Client.h"
#include "ClientController.h"

#pragma comment(lib,"Ws2_32.lib")

ClientController::ClientController()
{
	//�\�P�b�g������
	socket = std::make_shared<Socket>();
	socket = socket->
		SetProtocolVersion_IPv4().								//IPv4
		SetProtocol_TCP().										//TCP
		SetIpAddress("0,0,0,0").								//�A�h���X�w��
		SetPortNumber(PORTNUMBER).								//�|�[�g�ԍ�
		SetAsynchronous().										//�񓯊���
		ServerCreate(socket);									//�T�[�o�[�\�P�b�g����

}


ClientController::~ClientController()
{
	//�������
	for (int i = 0; i < socketList.size(); i++) {
		socketList[i]=nullptr;
	}
	socketList.clear();
	socket = nullptr;
}

void ClientController::Update(std::queue<NamedPipe::PipeData>* _dataList)
{
	AcceptSocket();
	ControllerUpdate();
	SocketUpdate();
	GetOutputPipeData(_dataList);
}

void ClientController::ControllerUpdate()
{
	//�ؒf�������s��ꂽ�\�P�b�g���폜
	int count = 0;
	for (int i = 0; i < socketList.size(); i++) {
		if (socketList[i]->GetState() == -1) {
			//�\�P�b�g�폜
			socketList[i] == nullptr;
			socketList.erase(socketList.begin() + i);
		}
	}
}

void ClientController::SocketUpdate()
{
	if (socketList.empty() == true)return;
	for (auto& client : socketList) {
		client->Update();
	}
}

void ClientController::GetOutputPipeData(std::queue<NamedPipe::PipeData>* _dataList)
{
	//�T�[�o�[�ɐڑ����Ă���N���C�A���g�����邩����
	if (socketList.empty() == true)return;

	//�p�C�v�f�[�^�̏���
	for (auto& client : socketList) {
		if (client->EmptyPipeData() == true)break;				//���S�f�[�^���Ȃ���Έȉ������͍s��Ȃ�
		NamedPipe::PipeData pipeData = *client->GetOutputPipeData();
		_dataList->push(pipeData);
		//�f�[�^�̍폜
		client->DeletePipeData();
	}

}

void ClientController::SendAllClient(char* _data, int _dataLength)
{
	for (auto client : socketList) {
		send(client->GetSocket(), _data,_dataLength, 0);
	}
}

void ClientController::SendAllClient(NamedPipe::PipeData* _data)
{
	for (auto client : socketList) {
		send(client->GetSocket(), _data->data, _data->byteSize, 0);
	}

}


void ClientController::AcceptSocket()
{
	std::shared_ptr<Client> clientSocket;											//�N���C�A���g�̃\�P�b�g�����ꎞ�I�ɕۑ�����ϐ�

	//accept
	SOCKET initSocket;
	initSocket = INVALID_SOCKET;													//client_socket������
	initSocket = socket->Accept();
	if (initSocket == INVALID_SOCKET)return;										//�A�N�Z�X���Ȃ������ꍇ�����ŏI���

	//�N���C�A���g�̃\�P�b�g���R���g���[���[�N���X�ɒǉ�����
	clientSocket = std::make_shared<Client>();
	clientSocket->GetData()->SetSocket(initSocket);									//�ڑ��̂������\�P�b�g���Z�b�g
	socketList.push_back(clientSocket);
	printf("LoadBalancer>>Access Socket\n");
}

