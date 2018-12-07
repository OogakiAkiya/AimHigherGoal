#ifndef OpenSSLRSA_h
#define OpenSSLRSA_h

class OpenSSLRSA {
public:
	OpenSSLRSA();
	~OpenSSLRSA();

	//---------------------------------------------------------
	//���ǂݎ��
	//---------------------------------------------------------
	void ReadPrivateKey(std::string _pass);
	void ReadPublicKey(std::string _pass);

	//---------------------------------------------------------
	//�Í����E����
	//---------------------------------------------------------
	int Encode(char* _encodedata,const char* _originaldata,const int _byte);				//�Í���
	int Decode(char* _decodedata, const char* _encodedata, const int _byte);				//����
private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	RSA* privateKey;
	RSA* publicKey;
};

#endif