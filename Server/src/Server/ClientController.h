#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update();
	void AddClient(std::shared_ptr<Client> _client);						//�\�P�b�g���X�g�ɒǉ�
	void ControllerUpdate();												//�X���b�h�����삳����֐�
	void SocketUpdate();
private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void DataManipulate(Client* _socket,std::vector<char>* _data);
	void DBCreateData();
	void DBGetPos(char* _data, std::shared_ptr<std::string> _userId);
	int CreateSendData(char* _encryptionData, Client* _socket, char* _originalData, int _dataLen);
	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::vector<std::shared_ptr<Client>> clientList;										//�\�P�b�g�z��
	std::vector<int> roomNumberList;														//�������ꂽ���[���ԍ���ۑ�
};

////////////////////////////////////////////////////////////////////
//�ȉ����M�f�[�^�\��
////////////////////////////////////////////////////////////////////
#pragma pack(1)
//�f�[�^���M�̍Œ��
struct tempData
{
	int size;
	char id;
};

//���[�U�[��񏈗�
struct TempUserData {
	tempData data;
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