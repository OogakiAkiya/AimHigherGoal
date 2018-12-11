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
	void SendAttack(Data* _data);

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
	//�R���X�g���N�^�[�E�f�X�g���N�^�[
	//---------------------------------------------------------
	void DataManipulate(const std::vector<char>* _data);				//�����i�f�[�^����ړ��Ȃǂ̏������s��	

	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void ExchangeKey();													//������

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int ATTACK = 11;										//�U��
	static const int DAMAGE = 13;										//�_���[�W���󂯂�
	static const int BYTESIZE = 256;									//����M�Ɏg�p����char�z��̃f�[�^��

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
struct BaseData
{
	int size;
	char id;
};

//���[�U�[���m�F����
struct UserData {
	BaseData base;
	int idsize;
	char id[20];
};

//���W�X�V�f�[�^
struct PosData
{
	BaseData base;
	float x;
	float y;
	float z;
	float angle;
	int animation;
};

//�U���f�[�^
struct AttckData
{
	BaseData base;
	int socket;
};

#pragma pack()

#endif