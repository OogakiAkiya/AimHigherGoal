class NamedPipe {
public:

#pragma pack(1)
	struct PipeData
	{
		int byteSize;
		char data[256];						//256�o�C�g+int�^��
	};
#pragma pack()

	NamedPipe();
	NamedPipe(std::string _discriminationName);
	~NamedPipe();
	bool CreateServer(std::string _pipeName);											//��M�p�p�C�v���쐬
	bool CreateClient(std::string _pipeName);											//���M�p�p�C�v���쐬
	bool ConnectRecv();																	//CreateServer�̌�ɐڑ����҂�
	int Read(char* _data, int _dataLength);												//_data�Ɏ�M�����f�[�^���󂯎��
	int Write(char* _data, int _dataLengeh);											//_data�𑗐M����
	void CreateInputPipe(std::string _pipeName,std::queue<PipeData>* _dataList);		//��M�p�̃X���b�h���쐬����
	void CountUp();																		//�J�E���g��1����������
	void CountDown();																	//�J�E���g��1����������
	int GetCount();																		//�J�E���g��Ԃ�
	std::string* GetPipeName();															//�p�C�v����Ԃ�
private:
	HANDLE pipeHandle = INVALID_HANDLE_VALUE;											//�p�C�v�n���h��
	std::string pipeName;																//�p�C�v��������
	std::string discriminationName = "";												//printf�̎��Ƀp�C�v�������ʂ���Ƃ��Ɏg�p����
	int count = 0;																		//�p�C�v�ɗ�����
	bool deleteFlg = false;																//�f�X�g���N�^��server��client���𔻒f����
};