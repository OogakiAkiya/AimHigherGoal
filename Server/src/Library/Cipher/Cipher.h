#ifndef Cipher_h
#define Cipher_h

class Cipher {
public:
	OpenSSLAES* GetOpenSSLAES();					//���ʌ�
	OpenSSLRSA* GetOpenSSLRSA();					//���J���Í�

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static Cipher & GetInstance();
	static void DeleteInstance();

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	Cipher();
	~Cipher();

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	OpenSSLAES* aes;								//���ʌ��Í�
	OpenSSLDH* dh;									//DH�@
	OpenSSLRSA* rsa;								//���J���Í�

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static Cipher* s_Instance;
	Cipher & operator=(Cipher &);
	Cipher(Cipher&);

};

#define CIPHER Cipher::GetInstance()
#endif