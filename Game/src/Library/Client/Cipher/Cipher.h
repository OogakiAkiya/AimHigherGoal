#ifndef Cipher_h
#define Cipher_h

class Cipher {
public:
	Cipher();
	~Cipher();

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	OpenSSLAES* GetOpenSSLAES();
	OpenSSLRSA* GetOpenSSLRSA();
private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<OpenSSLAES> aes=nullptr;				//���ʌ��Í�
	std::unique_ptr <OpenSSLRSA> rsa=nullptr;				//���J���Í�
};

#endif