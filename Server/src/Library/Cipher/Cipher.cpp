#include"../../Include.h"
#include"OpenSSLAES.h"
#include"OpenSSLRSA.h"
#include"Cipher.h"

Cipher* Cipher::s_Instance = nullptr;

Cipher::Cipher()
{
	//インスタンスの生成
	aes =std::make_shared<OpenSSLAES>();
	rsa = std::make_shared<OpenSSLRSA>();

	//秘密鍵読み込み
	rsa->ReadPrivateKey("key/private-key.pem");
}

Cipher::~Cipher()
{
	//解放処理
	aes = nullptr;
	rsa = nullptr;
}

std::shared_ptr<OpenSSLAES> Cipher::GetOpenSSLAES()
{
	return aes;
}

std::shared_ptr<OpenSSLRSA> Cipher::GetOpenSSLRSA()
{
	return rsa;
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
