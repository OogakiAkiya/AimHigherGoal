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
	int GetRoomNumber();
	std::shared_ptr<Data> GetData();
	int GetState();
	OpenSSLAES* GetAES();
	CurlWrapper* GetCurl();
	void SetSocket(SOCKET _socket);
	void SetNumber(int _number);
	void SetPosGetFlg();
	bool GetPosGetFlg();
	//---------------------------------------------------------
	//���S�f�[�^�������֐�
	//---------------------------------------------------------
	std::vector<char>* GetCompleteData();					//���S�f�[�^�̐擪�f�[�^�擾
	void DeleteCompleteData();								//���S�f�[�^�̐擪�f�[�^�폜
	bool EmptyCompleteData();								//���S�f�[�^�ɒl�������Ă��邩�ǂ������ׂ�(��Ȃ�:true,�l�������Ă����:false)

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void RecvLoop(int _loopType);
	bool ExchangeKey();
	void CreateCompleteData();

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 516;
	static const int EXCHANGEKEY = 1;
	static const int CREATECOMPLETEDATA = 2;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	int roomNumber=0;
	int state=0;															//�\�P�b�g�̏�Ԃ�\��(0:�ʏ�,-1:�f���[�g�҂�)
	bool keyChangeFlg = false;
	bool posGetFlg = false;
	std::unique_ptr<std::queue<std::vector<char>>> completeDataQueList;		//���S�f�[�^�z��
	std::vector<char> tempDataList;											//�ꎞ�f�[�^�z��
	std::shared_ptr<Data> data;												//���[�U�[�̊e�f�[�^���i�[����Ă���
	std::unique_ptr<OpenSSLAES> aes;										//���ʌ��Í��N���X
	std::unique_ptr<CurlWrapper> curl=nullptr;								//Http�ʐM�N���X
	std::unique_ptr<std::thread> thread = nullptr;
};

#endif