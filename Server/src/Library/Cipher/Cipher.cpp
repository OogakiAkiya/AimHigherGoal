#include"../../Include.h"
#include"OpenSSLAES.h"
#include"OpenSSLDH.h"
#include"Cipher.h"

Cipher* Cipher::s_Instance = nullptr;

Cipher::Cipher()
{
	/*�C���X�^���X�̐���*/
	aes = new OpenSSLAES();
	dh = new OpenSSLDH();
	//dh->DH_Init();
}

Cipher::~Cipher()
{
	/*�������*/
	delete aes;
	delete dh;
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
