#ifndef Client_h
#define Client_h

class Client
{
public:
	Client();
	~Client();

	void StartThread();
	bool CreateSocket(std::string _ip="127.0.0.1");
	void Client::Recv();
	//---------------------------------------------------------
	//���M�֐�
	//---------------------------------------------------------
	void SendUserInformation(Data* _data);
	void SendPos(Data* _data);

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	float GetCount();
	SOCKET GetSocket();
	Data GetData();
	Data* GetPlayerData();
	bool GetInitFlg();
	void DeleteData();
	void ClearData();
	bool DataEmpty();
	void SetEnemyData(int _enemyid, Data* data);

	//---------------------------------------------------------
	//�r������
	//---------------------------------------------------------
	void Lock();
	void Unlock();

	//---------------------------------------------------------
	//�����`���[
	//---------------------------------------------------------
	static void ClientThreadLauncher(void* args) {
		reinterpret_cast<Client*>(args)->Recv();						//�������void*�^�ɃL���X�g���āA�{���̏��������s����B
	}

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void ExchangeKey(std::string _id);
	void DataProcess(char _id, char * _data);

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int ATTACK = 11;										//�U��
	static const int DAMAGE = 13;										//�_���[�W���󂯂�
	static const int BYTESIZE = 256;									//����M�Ɏg�p����char�z��̃f�[�^��
	static const int RECVBYTESIZE = 1024*10;									//����M�Ɏg�p����char�z��̃f�[�^��

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	Cipher* cipher=nullptr;														//�Í�����
	ExtensionMutex* mutex = nullptr;												//�r������
	Socket* socket = nullptr;
	std::queue<Data> dataQueueList;										//�����i�f�[�^����쐬���ꂽ�e����ێ�
	std::vector<char> tempDataList;									//�ꎞ�I�Ƀf�[�^��ۑ�
	Data enemyData[3];													//�v���C���[�ȊO�̓G����ێ�
	std::shared_ptr<std::thread> thread=nullptr;
	Data* playerData;
	float count = 0;
	bool initFlag = false;

};


//=========================================================
//�ȉ����M�f�[�^�\��
//=========================================================
#pragma pack(1)
//�f�[�^���M�Ɏg���Œ���̍\��

struct Header
{
	int size;
	int playerIdSize;
	char playerId[20];
	unsigned char id;
};

struct BaseData
{
	int size;
	int playerIdSize;
	char playerId[20];
	char id;
};


//���W�X�V�f�[�^
struct PosData
{
	float x;
	float y;
	float z;
	float angle;
	int animation;
};
#pragma pack()

#endif