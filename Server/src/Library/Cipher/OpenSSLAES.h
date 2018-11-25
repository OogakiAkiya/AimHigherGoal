class OpenSSLAES {

public:
	OpenSSLAES();
	~OpenSSLAES();
	void CreateKey(const int _keyLength= EVP_MAX_KEY_LENGTH);										//ランダムなバイナリデータでのキーの生成
	int Encode(char* _encodeData, const char* _originalData, const size_t _dataLen);				//暗号化
	int Decode(char* _decodeData, const char* _originalData, const size_t _dataLen);				//復号
	
	void SetKey(unsigned char* _key,int _keyLength= EVP_MAX_KEY_LENGTH);
private:
	//---------------------------------------------------------
	//この関数でしか使用しない関数
	//---------------------------------------------------------
	int Multiple16(int _data);											//引数の値以上の最も近い値の16の倍数を返す

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	unsigned char* key;													//暗号化キー
	unsigned char* iv = (unsigned char*)"abcdefghijklmnop";				//初期ベクトル
	EVP_CIPHER_CTX* en;

};

