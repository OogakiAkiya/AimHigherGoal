#pragma once
class Client
{

public:
	Client();
	~Client();
	void Recv();											//��M����
	void StartRecvThread(Client* _client);						//�X���b�h�̊J�n
	void StartHttpThread();
	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	SOCKET GetSocket();
	int GetRoomNumber();
	Data* GetData();
	int GetState();
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
	//void RoomMatch();

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	int room_number=0;
	int state=0;											//�\�P�b�g�̏�Ԃ�\��(0:�ʏ�,-1:�f���[�g�҂�)
	std::thread* thread;
	std::queue<std::vector<char>> completedata_qlist;		//���S�f�[�^�z��
	std::vector<char> temp_datalist;						//�ꎞ�f�[�^�z��
	Data* data;												//���[�U�[�̊e�f�[�^���i�[����Ă���

	CurlWrapper* curl;
};

