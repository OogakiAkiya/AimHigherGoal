#include"../../StandardLibraryInclude.h"
#include"OpenSSLAES.h"
#include"OpenSSLRSA.h"
#include"Cipher.h"

Cipher::Cipher()
{
	aes = new OpenSSLAES();
	aes->CreateKey();
	rsa = new OpenSSLRSA();
	rsa->ReadPublicKey("key/public-key.pem");
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
