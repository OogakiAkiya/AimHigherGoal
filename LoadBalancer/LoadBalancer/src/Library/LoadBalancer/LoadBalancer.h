
class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
private:
	void CreateServerProcess();

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;						//�|�[�g�ԍ�
	const std::string INPUTPIPE = "AimHigherGoalInput";
	const std::string OUTPUTPIPE = "AimHigherGoalOutput";

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<Process> process;
	std::unique_ptr<ClientController> clientController;
	std::unique_ptr<std::queue<NamedPipe::PipeData>> sendDataQueue;								//�T�[�o�[�ɑ��M����f�[�^
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;								//�T�[�o�[����̃f�[�^����M����

	std::map<std::string, std::shared_ptr<NamedPipe>>outputPipeMap;	//�p�C�v��,�p�C�v
	std::map<std::string, int> serverList;						//�p�C�v��,�A�N�Z�X��
	std::map<std::string, SOCKET> socketList;					//�p�C�v��,SOCKET
	std::map<SOCKET, std::string>pipeList;						//SOCKET,�p�C�v��

};
