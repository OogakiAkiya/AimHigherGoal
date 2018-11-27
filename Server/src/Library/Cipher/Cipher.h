#ifndef Cipher_h
#define Cipher_h

class Cipher {
public:
	OpenSSLAES* GetOpenSSLAES();					//共通鍵
	OpenSSLRSA* GetOpenSSLRSA();					//公開鍵暗号

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static Cipher & GetInstance();
	static void DeleteInstance();

private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	Cipher();
	~Cipher();

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	OpenSSLAES* aes;								//共通鍵暗号
	OpenSSLDH* dh;									//DH法
	OpenSSLRSA* rsa;								//公開鍵暗号

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static Cipher* s_Instance;
	Cipher & operator=(Cipher &);
	Cipher(Cipher&);

};

#define CIPHER Cipher::GetInstance()
#endif