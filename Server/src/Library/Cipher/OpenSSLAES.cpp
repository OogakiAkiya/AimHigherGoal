#include"../../Include.h"
#include"OpenSSLAES.h"
#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")



OpenSSLAES::OpenSSLAES()
{
	//�C���X�^���X�̐���
	key = new unsigned char[EVP_MAX_KEY_LENGTH];
	en = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(en);								//EVP_CIPHER_CTX�̏�����
}

OpenSSLAES::~OpenSSLAES()
{
	//�������
	EVP_CIPHER_CTX_cleanup(en);								//EVP_CIPHER_CTX�̍폜
	EVP_CIPHER_CTX_free(en);
	delete key;
}

void OpenSSLAES::CreateKey(const int _keyLength)
{
	//�L�[�̐���
	int checkkey=RAND_bytes(key,_keyLength);						//���̃����_���쐬
	if (checkkey == 0) {
		checkkey = RAND_bytes(key, EVP_MAX_KEY_LENGTH);			//�\���Ȓ����������̃����_���쐬
	}
}

int OpenSSLAES::Encode(char* _encodeData,const char* _originalData, const size_t _dataLen/*�Í�������f�[�^�T�C�Y*/)
{
	char oriData[128];										//�Í�������f�[�^
	char enData[128];										//�Í����f�[�^
	int encodeSize = 0;										//�G���R�[�h�������̃o�C�g�T�C�Y
	int paddingSize = 0;									//�p�f�B���O�p�̃f�[�^������
	int outSize=0;											//�ŏI�I�ɏo�͂����f�[�^��
	int dataLength = Multiple16(_dataLen);					//16�̔{���ɂ��낦��

	memcpy(oriData, _originalData, _dataLen);				//���f�[�^�̃R�s�[

	//�Í�������
	EVP_EncryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);												//�Í����̏�����(�����x�N�g���͖���Ⴄ���̂�n��)AES��=EVP_aes_128_cbc()������Í����p���[�h�ɐݒ�
	EVP_EncryptUpdate(en, (unsigned char*)&enData, &encodeSize, (unsigned char*)&oriData,dataLength);		//�f�[�^�̈Í���(outlen�ɂ�16�o�C�g��n��)
	EVP_EncryptFinal_ex(en, (unsigned char*)&enData + encodeSize, &paddingSize);							//16�̔{���̃o�C�g���ɂȂ�悤�Ƀp�f�B���O�����
	outSize = encodeSize + paddingSize;																		//�ŏI�I�ȃf�[�^�ʂ̌v�Z
	memcpy(_encodeData,enData, outSize);																	//�Í��f�[�^�̃R�s�[

	return outSize;																							//�Í���������̃f�[�^�ʂ�Ԃ�
}

int OpenSSLAES::Decode(char* _decodeData, const char* _originalData, const size_t _dataLen/*��������f�[�^�T�C�Y*/)
{
	char oriData[128];									//��������f�[�^
	char deData[128];									//���������f�[�^
	int deSize=0;										//���������f�[�^��
	int outLength=0;									//�ŏI�I�ȏo�͂��ꂽ�f�[�^��
	int paddingSize = 0;								//�p�f�B���O���ꂽ�f�[�^��

	memcpy(oriData, _originalData, _dataLen);			//���f�[�^�̃R�s�[

	//��������
	EVP_DecryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);											//�����̏�����
	EVP_DecryptUpdate(en, (unsigned char*)&deData, &deSize, (unsigned char*)&oriData,_dataLen);			//�������̂܂܂���_datalen��48�̎�32�Ńf�R�[�h�����
	EVP_DecryptFinal_ex(en, (unsigned char*)&deData+deSize, &paddingSize);								//�I��
	outLength = deSize + paddingSize;																	//����������̃f�[�^��
	memcpy(_decodeData, deData, outLength);																//�����f�[�^�̃R�s�[

	return outLength;																					//����������̃f�[�^�ʂ�Ԃ�
 }

void OpenSSLAES::SetKey(unsigned char * _key,int _keyLength)
{
	//�����̒l�����ɐݒ肷��
	memcpy(key, _key, _keyLength);
	printf("���ʌ�=%s\n", key);
}

void OpenSSLAES::GetKey(char * _key)
{
	memcpy(_key, key, EVP_MAX_KEY_LENGTH);
}

int OpenSSLAES::Multiple16(int _data)
{
	//16�̔{���̏ꍇ���̂܂ܒl��Ԃ�
	if (_data % 16 == 0)return _data;

	//16�̔{���Œl��Ԃ�
	int remainder = 0;
	for (int i = 1; i < 16; i++) {
		if ((_data + i) % 16 == 0) {
			remainder = i;
			break;
		}
	}
	return _data + remainder;
}
