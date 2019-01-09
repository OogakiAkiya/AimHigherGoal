#include"../Include.h"
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

Server::Server()
{
	//�V���O���g���쐬
	Cipher::GetInstance();
	
	//�����_�������̏�����
	srand(time(0));

	//�\�P�b�g������
	socket = make_shared<Socket>();
	socket = socket->
		SetProtocolVersion_IPv4().								//IPv4
		SetProtocol_TCP().										//TCP
		SetIpAddress("0,0,0,0").								//�A�h���X�w��
		SetPortNumber("49155").									//�|�[�g�ԍ�
		SetAsynchronous().										//�񓯊���
		ServerCreate(socket);									//�T�[�o�[�\�P�b�g����
	clientController=std::make_shared<ClientController>();
	
}


Server::~Server()
{
	Cipher::DeleteInstance();
	socket = nullptr;
	clientController = nullptr;
}

void Server::AcceptLoop()
{
	while (1) {
		if (socket == nullptr)return;
		AcceptSocket();
		clientController->Update();
	}
}


void Server::AcceptSocket()
{
	std::shared_ptr<Client> clientSocket;																//�N���C�A���g�̃\�P�b�g�����ꎞ�I�ɕۑ�����ϐ�

	//accept
	SOCKET initSocket;
	initSocket = INVALID_SOCKET;					//client_socket������
	initSocket = socket->Accept();
	if (initSocket == INVALID_SOCKET)return;
	//�N���C�A���g�̃\�P�b�g���R���g���[���[�N���X�ɒǉ�����
	clientSocket = std::make_shared<Client>();
	clientSocket->SetSocket(initSocket);											//Socket�N���X�ɃN���C�A���g�̏���������
	clientController->SetSocket(clientSocket);										//������N���C�A���g����SocketController�N���X�ŊǗ�

	printf("���܂���\n");
}
