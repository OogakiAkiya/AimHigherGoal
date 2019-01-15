#ifndef Server_h
#define Server_h

class Server
{
public:
	Server(int _processNumber=0);
	~Server();
	void Update();
private:
	void CreatePipe(int _processNumber);
	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	const char* PORTNUMBER = "49155";						//ポート番号
	const std::string INPUTPIPE = "AimHigherGoalOutput";
	const std::string OUTPUTPIPE = "AimHigherGoalInput";

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::unique_ptr<ClientController> clientController = nullptr;
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;								//ロードバランサーに送信するデータ
	std::unique_ptr<NamedPipe> outputPipe;
	bool available = false;
};

#endif