#include"../../StandardLibraryInclude.h"
#include"OpenSSLAES.h"
#include"OpenSSLRSA.h"
#include"Cipher.h"

Cipher::Cipher()
{
	//共通鍵暗号
	aes = new OpenSSLAES();
	aes->CreateKey();									//キーの生成

	//公開鍵暗号
	rsa = new OpenSSLRSA();
	rsa->ReadPublicKey("key/public-key.pem");			//キーの読み込み
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
