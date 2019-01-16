#ifndef Client_h
#define Client_h

class Client
{
public:
	void StartThread();
	bool CreateSocket(std::string _userId,std::string _ip="127.0.0.1");
	void Client::Recv();
	//---------------------------------------------------------
	//���M�֐�
	//---------------------------------------------------------
	void SendUserInformation(Data* _data);
	void SendPos(Data* _data);
	void SendAttack(std::shared_ptr<Data> _data);

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
	void SetEnemyData(int _enemyid, std::shared_ptr<Data> data);

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
	void DataManipulate(char _id,char* _data);							//��M�f�[�^����ړ��Ȃǂ̏������s��	

	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void ExchangeKey(std::string _id);													//������

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int ATTACK = 11;										//�U��
	static const int DAMAGE = 13;										//�_���[�W���󂯂�
	static const int BYTESIZE = 256;									//����M�Ɏg�p����char�z��̃f�[�^��
	static const int ENEMYAMOUNT = 3;
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<Cipher> cipher = nullptr;														//�Í�����
	std::unique_ptr <ExtensionMutex> mutex = nullptr;												//�r������
	std::shared_ptr<Socket> socket = nullptr;
	std::shared_ptr <Data> enemyData[ENEMYAMOUNT];													//�v���C���[�ȊO�̓G����ێ�
	std::unique_ptr <Data> playerData=nullptr;
	std::shared_ptr <std::queue<Data>> dataQueueList=nullptr;										//�����i�f�[�^����쐬���ꂽ�e����ێ�
	std::vector<char> tempDataList;									//�ꎞ�I�Ƀf�[�^��ۑ�
	std::unique_ptr <std::thread> thread=nullptr;
	bool initFlag=false;
	float count = 0;

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
struct SendData
{
	int size;
	int idSize;
	char id[20];
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

//�U���f�[�^
struct AttckData
{
	BaseData base;
	int socket;
};

#pragma pack()

#endif