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
	void SetPosGetFlg();
	bool GetPosGetFlg();
	//---------------------------------------------------------
	//���S�f�[�^�������֐�
	//---------------------------------------------------------
	NamedPipe::PipeData* GetPipeData();					//���S�f�[�^�̐擪�f�[�^�擾
	void DeletePipeData();								//���S�f�[�^�̐擪�f�[�^�폜
	bool EmptyPipeData();								//���S�f�[�^�ɒl�������Ă��邩�ǂ������ׂ�(��Ȃ�:true,�l�������Ă����:false)

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void RecvLoop();

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
	std::vector<char> tempDataList;											//�ꎞ�f�[�^�z��
	std::shared_ptr<Data> data;												//���[�U�[�̊e�f�[�^���i�[����Ă���
	std::unique_ptr<std::queue<NamedPipe::PipeData>> pipeDataList;
};

#endif