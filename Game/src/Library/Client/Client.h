#ifndef Client_h
#define Client_h

class Client
{
public:
	void StartThread();
	void Recv();

	//---------------------------------------------------------
	//���M�֐�
	//---------------------------------------------------------
	void SendPos(Data* _data);
	void SendAttack(Data* _data);

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	float GetCount();
	SOCKET GetSocket();
	Data GetData();
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
	//�V���O���g��
	//---------------------------------------------------------
	static Client & GetInstance();
	static void DeleteInstance();

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
	Client();
	~Client();
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
	Cipher* cipher;														//�Í�����
	ExtensionMutex* mutex;												//�r������
	Socket* socket;
	std::queue<Data> dataQueueList;										//�����i�f�[�^����쐬���ꂽ�e����ێ�
	float count = 0;
	std::vector<char> tempDataList;									//�ꎞ�I�Ƀf�[�^��ۑ�
	Data enemyData[3];													//�v���C���[�ȊO�̓G����ێ�
	std::thread* thread;

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static Client* s_Instance;
	Client & operator=(Client &);										//����̐錾
	Client(Client&);													//�R�s�[�R���X�g���N�^

};

#define CLIENT Client::GetInstance()

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

//���W�X�V�f�[�^
struct PosData
{
	int size;
	char id;
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