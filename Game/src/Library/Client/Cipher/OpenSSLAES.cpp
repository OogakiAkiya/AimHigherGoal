#include"../../StandardLibraryInclude.h"
#include"OpenSSLAES.h"
#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")



OpenSSLAES::OpenSSLAES()
{
	key = new unsigned char[EVP_MAX_KEY_LENGTH];
	en = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(en);

}

OpenSSLAES::~OpenSSLAES()
{
	EVP_CIPHER_CTX_cleanup(en);
	EVP_CIPHER_CTX_free(en);
	delete key;
}

void OpenSSLAES::CreateKey(const int _keylength)
{
	int checkkey;												//���̏��ʂ��\�������f����
	checkkey=RAND_bytes(key,_keylength);						//���̃����_���쐬
	if (checkkey == 0) {
		checkkey = RAND_bytes(key, EVP_MAX_KEY_LENGTH);			//�\���Ȓ����������̃����_���쐬
	}
}

int OpenSSLAES::Encode(char* _encodedata,const char* _originaldata, const size_t _datalen/*�Í�������f�[�^�T�C�Y*/)
{
	char oridata[128];										//�Í�������f�[�^
	char endata[128];										//�Í����f�[�^
	int ensize = 0;											//�G���R�[�h�������̃o�C�g�T�C�Y
	int paddingsize = 0;									//�p�f�B���O�p�̃f�[�^������
	int outsize=0;											//�ŏI�I�ɏo�͂����f�[�^��
	int datalen = Multiple16(_datalen);						//�����̃f�[�^�ʂ�16�̔{���ɂ��낦��

	memcpy(oridata, _originaldata, _datalen);				//���f�[�^�̃R�s�[

	EVP_EncryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);															//�Í����̏�����(�����x�N�g���͖���Ⴄ���̂�n��)
	EVP_EncryptUpdate(en, (unsigned char*)&endata, &ensize, (unsigned char*)&oridata,datalen);				//�f�[�^�̈Í���(outlen�ɂ�16�o�C�g��n��)
	EVP_EncryptFinal_ex(en, (unsigned char*)&endata + ensize, &paddingsize);											//16�̔{���̃o�C�g���ɂȂ�悤�Ƀp�f�B���O�����

	outsize = ensize + paddingsize;							//�ŏI�I�ȃf�[�^�ʂ̌v�Z

	memcpy(_encodedata,endata, outsize);					//�f�[�^�̃R�s�[

	return outsize;
}

int OpenSSLAES::Decode(char* _decodedata, const char* _originaldata, const size_t _datalen/*��������f�[�^�T�C�Y*/)
{
	char oridata[128];												//���f�[�^
	char dedata[128];												//��������f�[�^
	int desize=0;
	int outlen=0;
	int paddingsize = 0;
	int outsize = 0;
	int erro;
	memcpy(oridata, _originaldata, _datalen);						//���f�[�^�̃R�s�[

	erro=EVP_DecryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);														//�����̏�����
	erro=EVP_DecryptUpdate(en, (unsigned char*)&dedata, &desize, (unsigned char*)&oridata,_datalen);						//����
	erro=EVP_DecryptFinal_ex(en, (unsigned char*)&dedata+desize, &paddingsize);											//�I��

	outlen = desize + paddingsize;
	memcpy(_decodedata, dedata, outlen);


	return outlen;
 }

void OpenSSLAES::GetKey(char * _key)
{
	memcpy(_key, key, EVP_MAX_KEY_LENGTH);
}


int OpenSSLAES::Multiple16(int _data)
{
	if (_data % 16 == 0)return _data;
	int remainder=0;
	for (int i = 1; i < 16; i++) {
		if ((_data + i) % 16 == 0) {
			remainder = i;
			break;
		}
	}
	return _data+remainder;
}
