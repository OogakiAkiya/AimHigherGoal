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
	OpenSSLAES* aes;				//���ʌ��Í�
	OpenSSLRSA* rsa;				//���J���Í�
};

#endif