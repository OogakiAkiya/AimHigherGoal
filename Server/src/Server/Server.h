#ifndef Server_h
#define Server_h

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
	Socket* socket = nullptr;
	std::shared_ptr<ClientController> clientController = nullptr;
	bool available = false;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

};

#endif