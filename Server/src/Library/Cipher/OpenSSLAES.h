#ifndef OpenSSLAES_h
#define OpenSSLAES_h

class OpenSSLAES {
public:
	OpenSSLAES();
	~OpenSSLAES();
	void CreateKey(const int _keyLength= EVP_MAX_KEY_LENGTH);										//�����_���ȃo�C�i���f�[�^�ł̃L�[�̐���
	int Encode(char* _encodeData, const char* _originalData, const size_t _dataLen);				//�Í���
	int Decode(char* _decodeData, const char* _originalData, const size_t _dataLen);				//����

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	void SetKey(unsigned char* _key,int _keyLength= EVP_MAX_KEY_LENGTH);							//���ʌ����Z�b�g����

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	int Multiple16(int _data);											//�����̒l�ȏ�̍ł��߂��l��16�̔{����Ԃ�

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	unsigned char* key;													//�Í����L�[
	unsigned char* iv = (unsigned char*)"abcdefghijklmnop";				//�����x�N�g��
	EVP_CIPHER_CTX* en;

};

#endif