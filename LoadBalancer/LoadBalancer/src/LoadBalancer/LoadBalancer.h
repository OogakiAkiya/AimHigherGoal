class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
	void CreateServer();
private:
	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;						//ポート番号

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------

	std::vector<PROCESS_INFORMATION> processList;
	int serverAmount=0;
	std::map<std::string, int> serverList;
};