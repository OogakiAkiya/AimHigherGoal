class LoadBalancer {
public:
	LoadBalancer();
	~LoadBalancer();
	void Updata();
private:
	void SendUpdate();														//�T�[�o�[�ւ̃f�[�^���M
	void RecvUpdate();														//�T�[�o�[����̃f�[�^��M
	void CreateServerProcess();												//�T�[�o�[�v���Z�X���쐬

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;									//�|�[�g�ԍ�
	static const int ACCESSLIMIT = 100;									//�T�[�o�[�������̃A�N�Z�X���E

	const std::string INPUTPIPE = "AimHigherGoalInput";
	const std::string OUTPUTPIPE = "AimHigherGoalOutput";

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<Process> process;										//�v���Z�X������
	std::unique_ptr<ClientController> clientController;						//�N���C�A���g�̃\�P�b�g�ꗗ������
	std::unique_ptr<std::queue<NamedPipe::PipeData>> sendDataQueue;			//�T�[�o�[�ɑ��M����f�[�^
	std::unique_ptr<std::queue<NamedPipe::PipeData>> recvDataQueue;			//�T�[�o�[����̃f�[�^����M����
	std::map<std::string, std::shared_ptr<NamedPipe>> outputPipeMap;		//�p�C�v��,�p�C�v
	std::map<std::string, std::string> userMap;								//���[�U�[��,�p�C�v��

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
