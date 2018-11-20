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

	
	std::string ori = "Hello";
	char encode[256];
	char decode[256];
	rsa->ReadPrivateKey("key/private-key.pem");
	rsa->ReadPublicKey("key/public-key.pem");
	int ensize=rsa->Encode(&encode[0],ori.c_str(),ori.size());
	rsa->Decode(decode,&encode[0],ensize);

	printf("\ndecode=%s", decode);
	
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
