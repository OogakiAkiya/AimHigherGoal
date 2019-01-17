class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
private:
	void SendUpdate();
	void RecvUpdate();
	void CreateServerProcess();

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;						//ポート番号
	static const int ACCESSLIMIT = 2000;						//ポート番号

	const std::string INPUTPIPE = "AimHigherGoalInput";
	const std::string OUTPUTPIPE = "AimHigherGoalOutput";

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	bool test = true;
	std::unique_ptr<Process> process;
	std::unique_ptr<ClientController> clientController;
	std::unique_ptr<std::queue<NamedPipe::PipeData>> sendDataQueue;								//サーバーに送信するデータ
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;								//サーバーからのデータを受信する
	std::map<std::string, std::shared_ptr<NamedPipe>> outputPipeMap;							//パイプ名,パイプ
	std::map<std::string, std::string> userMap;													//ユーザー名,パイプ名
};

#pragma pack(1)
struct Header
{
	int size;
	int playerIdSize;
	char playerId[20];
	unsigned char id;
};
#pragma pack()
