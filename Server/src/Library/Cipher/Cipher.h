class Cipher {
public:
	Cipher();
	~Cipher();
	OpenSSLAES* GetOpenSSLAES();

	//シングルトン=======================================================
	static Cipher & GetInstance();
	static void DeleteInstance();
	//===================================================================

private:

	//変数===============================================================
	OpenSSLAES* aes;								//共通鍵暗号
	OpenSSLDH* dh;									//DH法
	//===================================================================

	//シングルトン=======================================================
	static Cipher* s_Instance;
	Cipher & operator=(Cipher &);
	Cipher(Cipher&);
	//===================================================================

};
#define CIPHER Cipher::GetInstance()