#ifndef Client_h
#define Client_h

class Client
{
public:
	Client();
	~Client();
	void Update();

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	SOCKET GetSocket();
	std::shared_ptr<Data> GetData();
	int GetState();
	OpenSSLAES* GetAES();
	void SetPosGetFlg();
	bool GetPosGetFlg();
	//---------------------------------------------------------
	//���S�f�[�^�������֐�
	//---------------------------------------------------------
	std::vector<char>* GetCompleteData();					//���S�f�[�^�̐擪�f�[�^�擾
	void DeleteCompleteData();								//���S�f�[�^�̐擪�f�[�^�폜
	bool EmptyCompleteData();								//���S�f�[�^�ɒl�������Ă��邩�ǂ������ׂ�(��Ȃ�:true,�l�������Ă����:false)

	//new
	std::queue<NamedPipe::PipeData>* GetSendData();
	//---------------------------------------------------------
	//new�֐�
	//---------------------------------------------------------
	void AddData(NamedPipe::PipeData* _data);

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void RecvLoop(int _loopType);
	bool ExchangeKey();
	void CreateCompleteData();
	void DecryptionManipulate();
	void DataManipulater(char _id,char* _data);
	void DBCreateData();
	void DBGetPos(char* _data, std::shared_ptr<std::string> _userId);
	void CreateSendData(char* _originalData, int _dataLen, char _id);
	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 516;
	static const int EXCHANGEKEY = 1;
	static const int CREATECOMPLETEDATA = 2;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	int state=0;															//�\�P�b�g�̏�Ԃ�\��(0:�ʏ�,-1:�f���[�g�҂�)
	bool keyChangeFlg = false;
	bool posGetFlg = false;
	std::unique_ptr<std::queue<std::vector<char>>> completeDataQueList;		//���S�f�[�^�z��
	std::vector<char> tempDataList;											//�ꎞ�f�[�^�z��
	std::shared_ptr<Data> data;												//���[�U�[�̊e�f�[�^���i�[����Ă���
	std::unique_ptr<OpenSSLAES> aes;										//���ʌ��Í��N���X

	//new
	std::unique_ptr<std::queue<NamedPipe::PipeData>> tempDataQueue;			//�f�R�[�h�O�̃f�[�^
	std::unique_ptr<std::queue<NamedPipe::PipeData>> completeDataQueue;		//���M�f�[�^


};

#pragma pack(1)
struct BaseData
{
	int size;
	int playerIdSize;
	char playerId[20];
	char id;
};
struct Pos
{
	float x;
	float y;
	float z;
};

//���W�f�[�^
struct UserData {
	float x;
	float y;
	float z;
	float angle;
	int animation;
};

#pragma pack()


#endif