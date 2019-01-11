#ifndef Server_h
#define Server_h

class Server
{
public:
	Server(std::string _port="49155");
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
	//変数
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;
	std::unique_ptr<ClientController> clientController = nullptr;
	bool available = false;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

};

#endif