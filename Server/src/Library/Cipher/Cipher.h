class Cipher {
public:
	Cipher();
	~Cipher();
	OpenSSLAES* GetOpenSSLAES();

	//�V���O���g��=======================================================
	static Cipher & GetInstance();
	static void DeleteInstance();
	//===================================================================

private:

	//�ϐ�===============================================================
	OpenSSLAES* aes;								//���ʌ��Í�
	OpenSSLDH* dh;									//DH�@
	//===================================================================

	//�V���O���g��=======================================================
	static Cipher* s_Instance;
	Cipher & operator=(Cipher &);
	Cipher(Cipher&);
	//===================================================================

};
#define CIPHER Cipher::GetInstance()