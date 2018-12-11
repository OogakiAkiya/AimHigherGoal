#include"../../StandardLibraryInclude.h"
#include"OpenSSLAES.h"
#include"OpenSSLRSA.h"
#include"Cipher.h"

Cipher::Cipher()
{
	//���ʌ��Í�
	aes = std::make_unique<OpenSSLAES>();
	aes->CreateKey();									//�L�[�̐���

	//���J���Í�
	rsa = std::make_unique<OpenSSLRSA>();
	rsa->ReadPublicKey("key/public-key.pem");			//�L�[�̓ǂݍ���
}

Cipher::~Cipher()
{
	aes = nullptr;
	rsa = nullptr;
}

OpenSSLAES* Cipher::GetOpenSSLAES()
{
	return aes.get();
}

OpenSSLRSA * Cipher::GetOpenSSLRSA()
{
	return rsa.get();
}
