class OpenSSLRSA {
public:
	OpenSSLRSA();
	~OpenSSLRSA();

	void ReadPrivateKey(std::string _pass);
	void ReadPublicKey(std::string _pass);
	int Encode(char* _encodedata,const char* _originaldata,const int _byte);
	int Decode(char* _decodedata, const char* _encodedata, const int _byte);
private:
	RSA* privateKey;
	RSA* publicKey;
};