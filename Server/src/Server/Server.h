#ifndef Server_h
#define Server_h

class Server
{
public:
	Server();
	~Server();
	void AcceptLoop();										//メイン関数で呼ぶループ

private:

	//---------------------------------------------------------
	//Winsock2を使用したソケットプログラミング
	//---------------------------------------------------------
	void AcceptSocket();

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	const char* PORTNUMBER = "49155";						//ポート番号

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	Socket* socket = nullptr;
	std::shared_ptr<ClientController> clientController = nullptr;
	bool available = false;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

};

#endif