#ifndef Cipher_h
#define Cipher_h

class Cipher {
public:
	std::shared_ptr<OpenSSLAES> GetOpenSSLAES();					//共通鍵
	std::shared_ptr<OpenSSLRSA> GetOpenSSLRSA();					//公開鍵暗号

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
	std::shared_ptr<OpenSSLAES> aes;								//共通鍵暗号
	std::shared_ptr<OpenSSLRSA> rsa;								//公開鍵暗号

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static Cipher* s_Instance;
	Cipher & operator=(Cipher &);
	Cipher(Cipher&);

};

#define CIPHER Cipher::GetInstance()
#endif