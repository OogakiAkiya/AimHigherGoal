#ifndef OpenSSLRSA_h
#define OpenSSLRSA_h

class OpenSSLRSA {
public:
	OpenSSLRSA();
	~OpenSSLRSA();

	//---------------------------------------------------------
	//���ǂݍ���
	//---------------------------------------------------------
	void ReadPrivateKey(std::string _pass);											//�閧���ǂݍ���
	void ReadPublicKey(std::string _pass);											//���J���ǂݍ���

	//---------------------------------------------------------
	//�Í����E����
	//---------------------------------------------------------
	int Encode(char* _encodedata,const char* _originaldata,const int _byte);		//�Í���
	int Decode(char* _decodedata, const char* _encodedata, const int _byte);		//����

private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	RSA* privateKey;																//�閧��
	RSA* publicKey;																	//���J��
};

#endif