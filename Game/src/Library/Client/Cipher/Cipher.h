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
	std::unique_ptr<OpenSSLAES> aes=nullptr;				//‹¤’ÊŒ®ˆÃ†
	std::unique_ptr <OpenSSLRSA> rsa=nullptr;				//ŒöŠJŒ®ˆÃ†
};

#endif