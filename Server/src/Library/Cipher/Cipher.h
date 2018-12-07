#ifndef Cipher_h
#define Cipher_h

class Cipher {
public:
	std::shared_ptr<OpenSSLAES> GetOpenSSLAES();					//���ʌ�
	std::shared_ptr<OpenSSLRSA> GetOpenSSLRSA();					//���J���Í�

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
	std::shared_ptr<OpenSSLAES> aes;								//���ʌ��Í�
	std::shared_ptr<OpenSSLRSA> rsa;								//���J���Í�

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static Cipher* s_Instance;
	Cipher & operator=(Cipher &);
	Cipher(Cipher&);

};

#define CIPHER Cipher::GetInstance()
#endif