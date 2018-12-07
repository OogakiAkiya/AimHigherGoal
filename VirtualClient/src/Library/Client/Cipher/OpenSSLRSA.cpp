#include"../../StandardLibraryInclude.h"
#include"OpenSSLRSA.h"

OpenSSLRSA::OpenSSLRSA()
{

}

OpenSSLRSA::~OpenSSLRSA()
{
	RSA_free(privateKey);
	RSA_free(publicKey);
}


void OpenSSLRSA::ReadPrivateKey(std::string _pass)
{
	//�t�@�C���ǂݍ���
	BIO* file;
	file = BIO_new_file(_pass.c_str(), "r");

	if (!file) {
		printf("�閧���t�@�C���ǂݍ��݃G���[\n");
		return;
	}

	//�閧���ǂݍ���
	privateKey = PEM_read_bio_RSAPrivateKey(file, NULL, NULL, NULL);
	if (!privateKey) {
		printf("�閧���ǂݍ��݃G���[");
		return;
	}

	//�������
	BIO_free(file);
}

void OpenSSLRSA::ReadPublicKey(std::string _pass)
{
	//�t�@�C���ǂݍ���
	BIO* file = BIO_new_file(_pass.c_str(), "r");

	if (!file) {
		printf("���J���t�@�C���ǂݍ��݃G���[\n");
		return;
	}

	//���J���ǂݍ���
	EVP_PKEY* pkay = PEM_read_bio_PUBKEY(file, NULL, NULL, NULL);
	publicKey = EVP_PKEY_get1_RSA(pkay);
	if (!publicKey) {
		printf("���J���ǂݍ��݃G���[");
		return;
	}

	//�������
	BIO_free(file);
	EVP_PKEY_free(pkay);

}

int OpenSSLRSA::Encode(char * _encodedata, const char * _originaldata, const int _byte)
{
	char encode[256];						//�Í����f�[�^

	//�Í���
	int outlen = RSA_public_encrypt(_byte, (unsigned char*)_originaldata, (unsigned char*)encode, publicKey, RSA_PKCS1_PADDING);
	memcpy(_encodedata, encode, outlen);																								//�Í��f�[�^�̃R�s�[
	return outlen;
}

int OpenSSLRSA::Decode(char * _decodedata, const char * _encodedata, const int _byte)
{
	char decode[256];						//�����f�[�^

	//����
	int outlen = RSA_private_decrypt(_byte, (unsigned char*)_encodedata, (unsigned char*)decode, privateKey, RSA_PKCS1_PADDING);
	memcpy(_decodedata, decode, outlen);																								//�����f�[�^�̃R�s�[
	return outlen;
}
