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
	//�萔
	//---------------------------------------------------------
	static const int PORTNUMBER = 49155;						//�|�[�g�ԍ�
	const std::string INPUTPIPE = "AimHigherGoalInput";
	const std::string OUTPUTPIPE = "AimHigherGoalOutput";

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<Process> process;
	std::vector<OutputData> dataList;								//�N���C�A���g�ɑ��M����f�[�^

	std::map<std::string, std::shared_ptr<NamedPipe>>inputPipeMap;	//�p�C�v��,�p�C�v
	std::map<std::string, int> serverList;						//�p�C�v��,�A�N�Z�X��
	std::map<std::string, SOCKET> socketList;					//�p�C�v��,SOCKET
	std::map<SOCKET, std::string>pipeList;						//SOCKET,�p�C�v��

};