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
	//�萔
	//---------------------------------------------------------
	const std::string INPUTPIPE = "AimHigherGoalOutput";
	const std::string OUTPUTPIPE = "AimHigherGoalInput";

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::map<std::string, std::shared_ptr<Client>> clientMap;
	std::unique_ptr<std::queue<NamedPipe::PipeData>> sendDataQueue;								//���[�h�o�����T�[�ɑ��M����f�[�^
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;								//���[�h�o�����T�[����̃f�[�^����M����
	std::unique_ptr<NamedPipe> outputPipe;
	int processNumber;
	std::shared_ptr<std::string> query;																			//Http�ʐM�ő��M����f�[�^
	bool available = false;
};

#endif