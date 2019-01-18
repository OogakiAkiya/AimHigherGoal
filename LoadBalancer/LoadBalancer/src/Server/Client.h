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
	NamedPipe::PipeData* GetOutputPipeData();								//OutputPipe���M�p�̐擪�f�[�^�擾
	void DeletePipeData();													//OutputPipe���M�p�̐擪�f�[�^�폜
	bool EmptyPipeData();													//OutputPipe���M�p�ɒl�������Ă��邩�ǂ������ׂ�(��Ȃ�:true,�l�������Ă����:false)
private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void RecvLoop();														//��M����

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 516;										//��M�Ɏg�p����z��f�[�^�̃T�C�Y

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	int state=0;															//�\�P�b�g�̏�Ԃ�\��(0:�ʏ�,-1:�f���[�g�҂�)
	std::vector<char> tempDataList;											//�ꎞ�f�[�^�z��
	std::shared_ptr<Data> data;												//���[�U�[�̊e�f�[�^���i�[����Ă���
	std::shared_ptr<std::queue<NamedPipe::PipeData>> outputPipeDataList;	//outputPipe�ő��M����f�[�^���X�g
};

#endif