#ifndef OpenSSLAES_h
#define OpenSSLAES_h

class OpenSSLAES {
public:
	OpenSSLAES();
	~OpenSSLAES();

	//---------------------------------------------------------
	//鍵生成
	//---------------------------------------------------------
	void CreateKey(const int _keylength= EVP_MAX_KEY_LENGTH);

	//---------------------------------------------------------
	//暗号化・復号
	//---------------------------------------------------------
	int Encode(char* _encodedata, const char* _originaldata, const size_t _datalen);
	int Decode(char* _decodedata, const char* _originaldata, const size_t _datalen);

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	void GetKey(char* _key);
	void SetKey(unsigned char* _key, int _keyLength = EVP_MAX_KEY_LENGTH);							//共通鍵をセットする

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	int Multiple16(int _data);											//引数の値を16の倍数になるように計算する
	unsigned char* key;
	unsigned char* iv = (unsigned char*)"abcdefghijklmnop";
	EVP_CIPHER_CTX* en;

};

#endif