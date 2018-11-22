#include"../../StandardLibraryInclude.h"
#include"OpenSSLAES.h"
#include"OpenSSLRSA.h"
#include"Cipher.h"

Cipher::Cipher()
{
	//���ʌ��Í�
	aes = new OpenSSLAES();
	aes->CreateKey();									//�L�[�̐���

	//���J���Í�
	rsa = new OpenSSLRSA();
	rsa->ReadPublicKey("key/public-key.pem");			//�L�[�̓ǂݍ���
}

Cipher::~Cipher()
{
	delete aes;
	delete rsa;
}

OpenSSLAES* Cipher::GetOpenSSLAES()
{
	return aes;
}

OpenSSLRSA * Cipher::GetOpenSSLRSA()
{
	return rsa;
}
