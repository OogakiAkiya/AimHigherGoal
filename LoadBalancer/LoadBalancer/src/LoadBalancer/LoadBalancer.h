class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
	void CreateServer();
private:
	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;						//�|�[�g�ԍ�

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------

	std::vector<PROCESS_INFORMATION> processList;
	int serverAmount=0;
};