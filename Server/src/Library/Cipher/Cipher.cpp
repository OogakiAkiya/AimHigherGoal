#include"../../Include.h"
#include"OpenSSLAES.h"
#include"OpenSSLDH.h"
#include"OpenSSLRSA.h"
#include"Cipher.h"

Cipher* Cipher::s_Instance = nullptr;

Cipher::Cipher()
{
	/*インスタンスの生成*/
	aes = new OpenSSLAES();
	dh = new OpenSSLDH();
	rsa = new OpenSSLRSA();
	rsa->CreateKey("private.pem","public.pem");
}

Cipher::~Cipher()
{
	/*解放処理*/
	delete aes;
	delete dh;
	delete rsa;
}

OpenSSLAES* Cipher::GetOpenSSLAES()
{
	return aes;
}

Cipher & Cipher::GetInstance()
{
	if (s_Instance == nullptr) { s_Instance = new Cipher(); }
	return *s_Instance;
}

void Cipher::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}

}
