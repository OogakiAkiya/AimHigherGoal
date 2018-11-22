class OpenSSLAES {

public:
	OpenSSLAES();
	~OpenSSLAES();

	//---------------------------------------------------------
	//������
	//---------------------------------------------------------
	void CreateKey(const int _keylength= EVP_MAX_KEY_LENGTH);

	//---------------------------------------------------------
	//�Í����E����
	//---------------------------------------------------------
	int Encode(char* _encodedata, const char* _originaldata, const size_t _datalen);
	int Decode(char* _decodedata, const char* _originaldata, const size_t _datalen);

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	void GetKey(char* _key);
private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	int Multiple16(int _data);											//�����̒l��16�̔{���ɂȂ�悤�Ɍv�Z����
	unsigned char* key;
	unsigned char* iv = (unsigned char*)"abcdefghijklmnop";

	EVP_CIPHER_CTX* en;


};

