#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update();
	void SetSocket(std::shared_ptr<Client> _socket);						//�\�P�b�g���X�g�ɒǉ�
	bool SerchNumber(int _number);											//�����Ɠ������[���i���o�[�����݂��邩���ׂ�
	void ControllerThread();												//�X���b�h�����삳����֐�
	void SocketThread();
private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void DataManipulate(Client* _socket,std::vector<char>* _data);

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::vector<std::shared_ptr<Client>> socketList;										//�\�P�b�g�z��
	std::vector<std::shared_ptr<Client>> addSocketPool;										//�\�P�b�g�z��
	std::vector<int> roomNumberList;														//�������ꂽ���[���ԍ���ۑ�
	std::unique_ptr<std::thread> thread;													//�X���b�h
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