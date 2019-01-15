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
	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	const char* PORTNUMBER = "49155";						//�|�[�g�ԍ�
	const std::string INPUTPIPE = "AimHigherGoalOutput";
	const std::string OUTPUTPIPE = "AimHigherGoalInput";

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<ClientController> clientController = nullptr;
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;								//���[�h�o�����T�[�ɑ��M����f�[�^
	std::unique_ptr<NamedPipe> outputPipe;
	bool available = false;
};

#endif