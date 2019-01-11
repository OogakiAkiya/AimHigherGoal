#ifndef Server_h
#define Server_h

class Server
{
public:
	Server(std::string _port="49155");
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
	//�ϐ�
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;
	std::unique_ptr<ClientController> clientController = nullptr;
	bool available = false;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

};

#endif