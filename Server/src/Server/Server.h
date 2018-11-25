#pragma once

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
	Socket* socket;
	bool available=false;
	ClientController socketController;
	struct addrinfo *result = NULL;
	struct addrinfo hints;

};

