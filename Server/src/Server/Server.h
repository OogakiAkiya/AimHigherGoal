#pragma once

class Server
{
public:
	Server();
	~Server();
	void AcceptLoop();										//���C���֐��ŌĂԃ��[�v

private:

	//---------------------------------------------------------
	//Winsock2���g�p�����\�P�b�g�v���O���~���O
	//---------------------------------------------------------
	void AcceptSocket();

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	const char* PORTNUMBER = "49155";						//�|�[�g�ԍ�

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	//SOCKET m_socket;
	Socket* m_socket;
	bool is_available=false;
	ClientController socket_controller;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

};

