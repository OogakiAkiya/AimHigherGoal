#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"Socket.h"
#include"CurlWrapper.h"
#include"Client.h"
#include "ClientController.h"
#include "Server.h"

using namespace std;
#pragma comment(lib,"Ws2_32.lib")

Server::Server()
{
	//�V���O���g���쐬
	ExtensionMutex::GetInstance();								//mutex�Ǘ�class�C���X�^���X����
	Cipher::GetInstance();
	
	//�����_�������̏�����
	srand(time(0));

	//�\�P�b�g������
	socket = Socket::Instantiate()->
		SetProtocolVersion_IPv6().								//IPv6
		SetProtocol_TCP().										//TCP
		SetIpAddress("0.0.0.0").
		SetPortNumber("49155").									//�|�[�g�ԍ�
		ServerCreate();											//�T�[�o�[�\�P�b�g

	//�\�P�b�g�̊Ǘ��N���X�̃X���b�h�J�n
	socketController.StartThread(&socketController);

}


Server::~Server()
{
	socket->Close();
	ExtensionMutex::DeleteInstance();													//mutex�Ǘ�class�C���X�^���X�̍폜
	Cipher::DeleteInstance();
	delete socket;
}

void Server::AcceptLoop()
{
	while (1) {
		AcceptSocket();
	}
}


void Server::AcceptSocket()
{
	Client* clientSocket;																//�N���C�A���g�̃\�P�b�g�����ꎞ�I�ɕۑ�����ϐ�

	//accept
	SOCKET initSocket;
	initSocket = INVALID_SOCKET;					//client_socket������
	initSocket = socket->Accept();

		/*�N���C�A���g�̃\�P�b�g���R���g���[���[�N���X�ɒǉ�����*/
		clientSocket = new Client();
		clientSocket->SetSocket(initSocket);											//Socket�N���X�ɃN���C�A���g�̏���������
		MUTEX.Lock();																	//�r������Lock
		socketController.SetSocket(clientSocket);										//������N���C�A���g����SocketController�N���X�ŊǗ�
		MUTEX.Unlock();																	//�r������Unlock

		printf("���܂���\n");
		clientSocket->StartRecvThread(clientSocket);									//recv�����̃X���b�h�J�n
		//clientSocket->StartHttpThread();												//Http�ʐM�̃X���b�h�J�n
}
