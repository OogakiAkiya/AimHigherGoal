#ifndef Client_h
#define Client_h

struct Header;

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
	OpenSSLAES* GetAES();
	void SetPosGetFlg();
	bool GetPosGetFlg();

	std::queue<NamedPipe::PipeData>* GetSendData();
	void AddData(NamedPipe::PipeData* _data);

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void DataProcess(Header* _header);
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
	bool posGetFlg = false;
	std::shared_ptr<Data> data;												//���[�U�[�̊e�f�[�^���i�[����Ă���
	std::unique_ptr<OpenSSLAES> aes;										//���ʌ��Í��N���X
	std::unique_ptr<std::queue<NamedPipe::PipeData>> tempDataQueue;			//�f�R�[�h�O�̃f�[�^
	std::unique_ptr<std::queue<NamedPipe::PipeData>> completeDataQueue;		//���M�f�[�^


};

#pragma pack(1)
struct Header
{
	int size;
	int playerIdSize;
	char playerId[20];
	unsigned char id;
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