#include"../../StandardLibraryInclude.h"
#include"OpenSSLAES.h"
#include"Cipher.h"

Cipher::Cipher()
{
	aes = new OpenSSLAES();
}

Cipher::~Cipher()
{
	delete aes;
}

OpenSSLAES* Cipher::GetOpenSSLAES()
{
	return aes;
}
