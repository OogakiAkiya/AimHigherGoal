#include"../../StandardLibraryInclude.h"
#include"OpenSSLAES.h"
#include"OpenSSLRSA.h"
#include"Cipher.h"

Cipher::Cipher()
{
	//共通鍵暗号
	aes = std::make_unique<OpenSSLAES>();
	aes->CreateKey();									//キーの生成

	//公開鍵暗号
	rsa = std::make_unique<OpenSSLRSA>();
	rsa->ReadPublicKey("key/public-key.pem");			//キーの読み込み
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
