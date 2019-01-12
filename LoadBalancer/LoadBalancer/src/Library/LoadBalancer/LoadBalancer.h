
class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
	void Temp();
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
	std::unique_ptr<std::queue<NamedPipe::PipeData>> dataList;								//クライアントに送信するデータ
	std::map<std::string, std::shared_ptr<NamedPipe>>inputPipeMap;	//パイプ名,パイプ
	std::map<std::string, int> serverList;						//パイプ名,アクセス数
	std::map<std::string, SOCKET> socketList;					//パイプ名,SOCKET
	std::map<SOCKET, std::string>pipeList;						//SOCKET,パイプ名
};