#ifndef Client_h
#define Client_h

class Client
{
public:
	Client();
	~Client();
	void Recv();											//��M����
	void StartRecvThread(Client* _client);					//Recv�X���b�h�̊J�n
	void StartHttpThread();									//Http�ʐM�X���b�h�J�n

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	SOCKET GetSocket();
	int GetRoomNumber();
	Data* GetData();
	int GetState();
	OpenSSLAES* GetAES();
	void SetSocket(SOCKET _socket);
	void SetNumber(int _number);
	
	//---------------------------------------------------------
	//���S�f�[�^�������֐�
	//---------------------------------------------------------
	std::vector<char>* GetCompleteData();					//���S�f�[�^�̐擪�f�[�^�擾
	void DeleteCompleteData();								//���S�f�[�^�̐擪�f�[�^�폜
	bool EmptyCompleteData();								//���S�f�[�^�ɒl�������Ă��邩�ǂ������ׂ�(��Ȃ�:true,�l�������Ă����:false)
	
	//---------------------------------------------------------
	//�����`���[
	//---------------------------------------------------------
	static void RecvLauncher(void* args) {
		reinterpret_cast<Client*>(args)->Recv();			// �������void*�^�ɃL���X�g���āA�{���̏��������s����B
	}

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
	static const int BYTESIZE = 256;
	static const int EXCHANGEKEY = 1;
	static const int CREATECOMPLETEDATA = 2;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	int roomNumber=0;
	int state=0;											//�\�P�b�g�̏�Ԃ�\��(0:�ʏ�,-1:�f���[�g�҂�)
	std::thread* thread;
	std::queue<std::vector<char>> completeDataQueList;		//���S�f�[�^�z��
	std::vector<char> tempDataList;							//�ꎞ�f�[�^�z��
	Data* data;												//���[�U�[�̊e�f�[�^���i�[����Ă���
	OpenSSLAES* aes;										//���ʌ��Í��N���X
	CurlWrapper* curl;										//Http�ʐM�N���X
};

#endif