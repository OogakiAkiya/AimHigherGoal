#ifndef CurlWrapper_h
#define CurlWrapper_h

class CurlWrapper
{
public:
	CurlWrapper();
	~CurlWrapper();

	//---------------------------------------------------------
	//�f�[�^�̑���M
	//---------------------------------------------------------
	void HTTPConnect(std::string* _data, std::string _url,std::string _postData);		//HTTP�ʐM���s��
	void PosUpdataLoop(std::shared_ptr<Data> _data);									//���W�̍X�V����
	void StartThread(std::shared_ptr<CurlWrapper> _curl, std::shared_ptr<Data> _data);	//�X���b�h�J�n����
	void CurlWrapper::DBGetPos(char* _data, std::shared_ptr<std::string> _userId);		//DB����l���󂯎��������Ɋi�[����

	//---------------------------------------------------------
	//�����`���[
	//---------------------------------------------------------
	static void HttpLauncher(void* _args,std::shared_ptr<Data> _data) {
		reinterpret_cast<CurlWrapper*>(_args)->PosUpdataLoop(_data);			// �������void*�^�ɃL���X�g���āA�{���̏��������s����B
	}

private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	CURL* curl;																//URL�ւ̃A�N�Z�X�ɕK�v�Ȑݒ�Ȃǂ�����
	CURLcode code;															//�����ɐ����������G���[�`�F�b�N�Ɏg��
	bool endFlg=true;
	std::shared_ptr <std::thread> thread;														//�ʃX���b�h����
	std::shared_ptr<std::string> userId;
};

struct RecvBuffer {
	std::vector<char>* data;
	int dataSize;
};

#endif