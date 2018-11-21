class Cipher {
public:
	Cipher();
	~Cipher();
	OpenSSLAES* GetOpenSSLAES();
	OpenSSLRSA* GetOpenSSLRSA();
private:
	OpenSSLAES* aes;
	OpenSSLRSA* rsa;
};