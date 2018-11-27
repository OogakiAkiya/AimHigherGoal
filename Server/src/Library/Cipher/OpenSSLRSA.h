#ifndef OpenSSLRSA_h
#define OpenSSLRSA_h

class OpenSSLRSA {
public:
	OpenSSLRSA();
	~OpenSSLRSA();

	//---------------------------------------------------------
	//Œ®“Ç‚İ‚İ
	//---------------------------------------------------------
	void ReadPrivateKey(std::string _pass);											//”é–§Œ®“Ç‚İ‚İ
	void ReadPublicKey(std::string _pass);											//ŒöŠJŒ®“Ç‚İ‚İ

	//---------------------------------------------------------
	//ˆÃ†‰»E•œ†
	//---------------------------------------------------------
	int Encode(char* _encodedata,const char* _originaldata,const int _byte);		//ˆÃ†‰»
	int Decode(char* _decodedata, const char* _encodedata, const int _byte);		//•œ†

private:
	//---------------------------------------------------------
	//•Ï”
	//---------------------------------------------------------
	RSA* privateKey;																//”é–§Œ®
	RSA* publicKey;																	//ŒöŠJŒ®
};

#endif