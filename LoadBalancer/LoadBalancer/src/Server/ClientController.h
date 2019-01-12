#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update(std::queue<NamedPipe::PipeData>* _dataList);
	void ControllerUpdate();												//�X���b�h�����삳����֐�
	void SocketUpdate();
	void GetPipeData(std::queue<NamedPipe::PipeData>* _dataList);
private:
	void AcceptSocket();


	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 128;
	const char* PORTNUMBER = "49155";						//�|�[�g�ԍ�


	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;
	std::vector<std::shared_ptr<Client>> socketList;										//�\�P�b�g�z��
};

////////////////////////////////////////////////////////////////////
//�ȉ����M�f�[�^�\��
////////////////////////////////////////////////////////////////////
#pragma pack(1)
//�f�[�^���M�̍Œ��
struct BaseData
{
	int size;
	char id;
};

//���[�U�[��񏈗�
struct UserData {
	BaseData data;
	float x;
	float y;
	float z;
};


//���W�f�[�^
struct PosData {
	int size;
	char id;
	float x;
	float y;
	float z;
	float angle;
	int animation;
};
#pragma pack()

#endif