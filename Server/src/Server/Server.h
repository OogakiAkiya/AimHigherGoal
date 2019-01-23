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
	void ClientUpdate();
	void InputPipeProcessing();
	void DBRegistration();
	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	const std::string INPUTPIPE = "AimHigherGoalOutput";
	const std::string OUTPUTPIPE = "AimHigherGoalInput";

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::map<std::string, std::shared_ptr<Client>> clientMap;
	std::unique_ptr<std::queue<NamedPipe::PipeData>> sendDataQueue;								//ロードバランサーに送信するデータ
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;								//ロードバランサーからのデータを受信する
	std::unique_ptr<NamedPipe> outputPipe;
	int processNumber;
	std::shared_ptr<std::string> query;																			//Http通信で送信するデータ
	bool available = false;
};

#endif