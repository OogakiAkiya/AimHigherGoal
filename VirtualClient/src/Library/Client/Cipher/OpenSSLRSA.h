#ifndef OpenSSLRSA_h
#define OpenSSLRSA_h

class OpenSSLRSA {
public:
	OpenSSLRSA();
	~OpenSSLRSA();

	//---------------------------------------------------------
	//Œ®“Ç‚İæ‚è
	//---------------------------------------------------------
	void ReadPrivateKey(std::string _pass);
	void ReadPublicKey(std::string _pass);

	//---------------------------------------------------------
	//ˆÃ†‰»E•œ†
	//---------------------------------------------------------
	int Encode(char* _encodedata,const char* _originaldata,const int _byte);				//ˆÃ†‰»
	int Decode(char* _decodedata, const char* _encodedata, const int _byte);				//•œ†
private:
	//---------------------------------------------------------
	//•Ï”
	//---------------------------------------------------------
	RSA* privateKey;
	RSA* publicKey;
};

#endif