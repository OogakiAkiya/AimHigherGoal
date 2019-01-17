struct UserData;
class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
private:
	void CreateServerProcess();

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;						//ポート番号
	const std::string INPUTPIPE = "AimHigherGoalInput";
	const std::string OUTPUTPIPE = "AimHigherGoalOutput";

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::unique_ptr<Process> process;
	std::unique_ptr<ClientController> clientController;
	std::unique_ptr<std::queue<NamedPipe::PipeData>> sendDataQueue;								//サーバーに送信するデータ
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;								//サーバーからのデータを受信する

	//使ってる
	std::map<std::string, std::shared_ptr<NamedPipe>>outputPipeMap;	//パイプ名,パイプ
	//まだ使ってない
	std::map<std::string, int> serverList;						//パイプ名,アクセス数
	std::map<std::string, SOCKET> socketList;					//パイプ名,SOCKET
	std::map<std::string, UserData>userMap;						//ユーザー名,ユーザーデータ
};

struct UserData{
	std::string pipeName;
	SOCKET socket;
};