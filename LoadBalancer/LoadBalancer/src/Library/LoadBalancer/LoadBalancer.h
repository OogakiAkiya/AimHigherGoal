class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
private:
	void SendUpdate();														//サーバーへのデータ送信
	void RecvUpdate();														//サーバーからのデータ受信
	void CreateServerProcess();												//サーバープロセスを作成

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;									//ポート番号
	static const int ACCESSLIMIT = 100;									//サーバー一つ当たりのアクセス限界

	const std::string INPUTPIPE = "AimHigherGoalInput";
	const std::string OUTPUTPIPE = "AimHigherGoalOutput";

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::unique_ptr<Process> process;										//プロセスを扱う
	std::unique_ptr<ClientController> clientController;						//クライアントのソケット一覧を扱う
	std::unique_ptr<std::queue<NamedPipe::PipeData>> sendDataQueue;			//サーバーに送信するデータ
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;			//サーバーからのデータを受信する
	std::map<std::string, std::shared_ptr<NamedPipe>> outputPipeMap;		//パイプ名,パイプ
	std::map<std::string, std::string> userMap;								//ユーザー名,パイプ名

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
