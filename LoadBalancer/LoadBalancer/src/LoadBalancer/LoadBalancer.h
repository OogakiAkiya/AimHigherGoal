class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
	void CreateServer();
private:
	//---------------------------------------------------------
	//íËêî
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;						//É|Å[Égî‘çÜ

	//---------------------------------------------------------
	//ïœêî
	//---------------------------------------------------------

	std::vector<PROCESS_INFORMATION> processList;
	int serverAmount=0;
};