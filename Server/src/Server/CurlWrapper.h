#ifndef CurlWrapper_h
#define CurlWrapper_h

class CurlWrapper
{
public:
	CurlWrapper();
	~CurlWrapper();
	void HttpConnect(Data* _data);											//Http�ʐM���s��
	void CurlWrapper::StartThread(CurlWrapper* _curl, Data* _data);			//�X���b�h�J�n����
	void DBGetPos(char* _data, std::string _userId);
	//---------------------------------------------------------
	//�����`���[
	//---------------------------------------------------------
	static void HttpLauncher(void* _args,Data* _data) {
		reinterpret_cast<CurlWrapper*>(_args)->HttpConnect(_data);			// �������void*�^�ɃL���X�g���āA�{���̏��������s����B
	}

private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	CURL* curl;																//URL�ւ̃A�N�Z�X�ɕK�v�Ȑݒ�Ȃǂ�����
	CURLcode code;															//�����ɐ����������G���[�`�F�b�N�Ɏg��
	std::thread thread;												//�ʃX���b�h����
};

struct RecvBuffer {
	std::vector<char>* data;
	int dataSize;
};

#endif