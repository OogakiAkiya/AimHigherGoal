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
	bool HTTPConnect(std::string* _data, std::string _url, std::string _postData);		//HTTP�ʐM���s��

private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	CURL* curl;																//URL�ւ̃A�N�Z�X�ɕK�v�Ȑݒ�Ȃǂ�����
	CURLcode code;															//�����ɐ����������G���[�`�F�b�N�Ɏg��
};

#endif