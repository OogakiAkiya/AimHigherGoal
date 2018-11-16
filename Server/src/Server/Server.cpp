#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
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
	//�V���O���g���쐬*/
	ExtensionMutex::GetInstance();								//mutex�Ǘ�class�C���X�^���X����
	Cipher::GetInstance();
	/*�����_�������̏�����*/
	srand(time(0));

	/*�\�P�b�g������*/
	m_socket = Socket::Instantiate()->
		SetProtocolVersion_IPv6().								//IPv6
		SetProtocol_TCP().										//TCP
		SetIpAddress("0.0.0.0").
		SetPortNumber("49155").									//�|�[�g�ԍ�
		ServerCreate();											//�T�[�o�[�\�P�b�g

	/*�\�P�b�g�̊Ǘ��N���X�̃X���b�h�J�n*/
	socket_controller.StartThread(&socket_controller);

}


Server::~Server()
{
	m_socket->Close();
	ExtensionMutex::DeleteInstance();													//mutex�Ǘ�class�C���X�^���X�̍폜
	Cipher::DeleteInstance();
	delete m_socket;
}

void Server::AcceptLoop()
{
	while (1) {
		AcceptSocket();
	}
}


void Server::AcceptSocket()
{
	Client* client_socket;																//�N���C�A���g�̃\�P�b�g�����ꎞ�I�ɕۑ�����ϐ�

	/*accept*/
	SOCKET init_socket;
	init_socket = INVALID_SOCKET;					//client_socket������
	init_socket = m_socket->Accept();

		/*�N���C�A���g�̃\�P�b�g���R���g���[���[�N���X�ɒǉ�����*/
		client_socket = new Client();
		client_socket->SetSocket(init_socket);											//Socket�N���X�ɃN���C�A���g�̏���������
		MUTEX.Lock();																	//�r������Lock
		socket_controller.SetSocket(client_socket);										//������N���C�A���g����SocketController�N���X�ŊǗ�
		MUTEX.Unlock();																	//�r������Unlock

		printf("���܂���\n");
		client_socket->StartRecvThread(client_socket);										//recv�����̃X���b�h�J�n
		client_socket->StartHttpThread();												//Http�ʐM�̃X���b�h�J�n
}


//void ClientRoutine(Socket _socket) { ; }
//{
//	SOCKET client=(SOCKET)ptr;			//�󂯎�����N���C�A���g����ۑ�
//	char receivbuf[512];					//�󂯎��o�b�t�@�T�C�Y��[]�̒��Ɏw��
//	int iResult;
//	for (auto element : roomlist) {
//		if (element.SerchSocket(client) == true) {
//			element.RoomMatchUpdate(client);
//		}
//
//	}
//
//	
//	while (1) 
//	{
//		/*��M*/
//		iResult= recv(client/*���M���̃\�P�b�g*/, receivbuf/*�f�[�^*/, 512/*�f�[�^�̃o�b�t�@�T�C�Y*/, 0);			//�f�[�^�󂯎��
//		if (iResult > 0) {
//			switch (receivbuf[0]) {
//			case 0x01: {										//���[���𗧂Ă�
//				int room_num;												//���[���ԍ�
//				while (1) {
//					bool breakflg=true;
//					room_num = (int)floor(rand() % 900000 + 100000);			//6���̔ԍ����쐬
//					for (auto element : roomlist) {
//						if (element.GetRoomNumber() == room_num)breakflg = false;
//					}
//					if (breakflg == true)break;
//				}
//				Room* newroom = new Room();					//���[���쐬		
//				newroom->SetRoomNumber(room_num);
//				newroom->SetMasterSocket((SOCKET)client);
//				roomlist.push_back(*newroom);
//				delete newroom;
//				break;
//			}
//
//			case 0x03: {															//���[���ɎQ��
//				bool serchflg = false;												//
//				int received_num = CharToNum(&receivbuf[1], 3);						//�󂯎�����l��10�i���ɕϊ�
//				for (auto element : roomlist) {										//���[���i���o�[�̒T��
//					if (element.GetRoomNumber() == received_num) {
//						element.SetSocket((SOCKET)client);							//�\�P�b�g��Room�N���X�ɃZ�b�g(Room�ɋ󂫂�����Βǉ��A�󂫂��Ȃ���΂Ȃ��ƒʒm)
//						serchflg = true;
//						break;
//					}
//				}
//				if (serchflg == false) {
//					//���[�����Ȃ�
//					char secondbuf[3];
//					secondbuf[0] = 0x05;
//					secondbuf[1] = 0x02;
//					secondbuf[2] = 0xff;
//					iResult = send(client, secondbuf, 3, 0);
//				}
//				break;
//			}
//			case 0x06:									//�}�b�`���O�L�����Z��
//				//for (auto element : roomlist) {
//					//element.DeleteSocket((SOCKET)ptr);
//					break;
//				//}
//			}
//
//			/*���M*/
//			iResult = send(client, receivbuf, 512, 0);
//
//		}else if (iResult == 0) {								//�M���������Ă��Ȃ������Ƃ�
//			printf("�ؒf����܂���\n");
//			for (auto element : roomlist) {
//				if (element.SerchSocket(client) == true) {
//					element.DeleteSocket((SOCKET)client);
//					break;
//				}
//			}
//			//�ؒf�����\�P�b�g�̍폜
//			auto itr = find(sockets.begin(), sockets.end(), client);							//�\�P�b�g��T��
//			sockets.erase(remove(sockets.begin(), sockets.end(), distance(sockets.begin(), itr)), sockets.end());			//��ŒT�������̂��폜
//			break;
//		}
//		else {														//����ȊO�̎�
//			printf("recv failed:%d\n", WSAGetLastError());
//			break;
//		}
//		
//	}
//}
/*
// buf����bytes����ǂݍ����int�ŕԂ�
int CharToNum(char *buf, int bytes) {
	int ret = 0;
	int p = 0;
	while (p < bytes) {
		ret += (ret * 0xff) + (unsigned char)buf[p++];
	}
	return ret;
}

// num��bytes������buf�Ƀo�C�i���ŏ�������
void NumToChar(int num, int bytes, char *buf) {
	int p = 0;
	while (p < bytes) {
		buf[bytes - (p++) - 1] = (num % 0x100) & 0xFF;
		num /= 0x100;
	}
}
*/