#ifndef Cipher_h
#define Cipher_h

class Cipher {
public:
	Cipher();
	~Cipher();

	//---------------------------------------------------------
	//î•ñæ“¾
	//---------------------------------------------------------
	OpenSSLAES* GetOpenSSLAES();
	OpenSSLRSA* GetOpenSSLRSA();
private:
	//---------------------------------------------------------
	//•Ï”
	//---------------------------------------------------------
	OpenSSLAES* aes;				//‹¤’ÊŒ®ˆÃ†
	OpenSSLRSA* rsa;				//ŒöŠJŒ®ˆÃ†
};

#endif