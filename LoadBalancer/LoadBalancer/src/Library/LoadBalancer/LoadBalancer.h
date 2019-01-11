#pragma pack(1)

struct OutputData
{
	int byteSize;
	char data[256];
};

#pragma pack()

class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
private:
	void CreateServerProcess();
	void CreateInputPipe(int _pipeNumber);
	void CreateOutputPipe(int _pipeNumber);

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
	std::vector<OutputData> dataList;								//クライアントに送信するデータ

	std::map<std::string, std::shared_ptr<NamedPipe>>inputPipeMap;	//パイプ名,パイプ
	std::map<std::string, int> serverList;						//パイプ名,アクセス数
	std::map<std::string, SOCKET> socketList;					//パイプ名,SOCKET
	std::map<SOCKET, std::string>pipeList;						//SOCKET,パイプ名

};