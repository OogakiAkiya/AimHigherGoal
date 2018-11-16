class Cipher {
public:
	Cipher();
	~Cipher();
	OpenSSLAES* GetOpenSSLAES();
private:
	OpenSSLAES* aes;
};