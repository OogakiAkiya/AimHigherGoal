class OpenSSLAES {

public:
	OpenSSLAES();
	~OpenSSLAES();

	void CreateKey(const int _keylength= EVP_MAX_KEY_LENGTH);
	int Encode(char* _encodedata, const char* _originaldata, const size_t _datalen);
	int Decode(char* _decodedata, const char* _originaldata, const size_t _datalen);
	void GetKey(char* _key);
private:

	int Multiple16(int _data);											//引数の値を16の倍数になるように計算する
	unsigned char* key;
	unsigned char* iv = (unsigned char*)"abcdefghijklmnop";

	EVP_CIPHER_CTX* en;


};

