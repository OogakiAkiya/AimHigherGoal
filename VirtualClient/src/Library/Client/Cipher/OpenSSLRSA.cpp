#include"../../StandardLibraryInclude.h"
#include"OpenSSLRSA.h"

OpenSSLRSA::OpenSSLRSA()
{

}

OpenSSLRSA::~OpenSSLRSA()
{
	RSA_free(privateKey);
	RSA_free(publicKey);
}


void OpenSSLRSA::ReadPrivateKey(std::string _pass)
{
	//ファイル読み込み
	BIO* file;
	file = BIO_new_file(_pass.c_str(), "r");

	if (!file) {
		printf("秘密鍵ファイル読み込みエラー\n");
		return;
	}

	//秘密鍵読み込み
	privateKey = PEM_read_bio_RSAPrivateKey(file, NULL, NULL, NULL);
	if (!privateKey) {
		printf("秘密鍵読み込みエラー");
		return;
	}

	//解放処理
	BIO_free(file);
}

void OpenSSLRSA::ReadPublicKey(std::string _pass)
{
	//ファイル読み込み
	BIO* file = BIO_new_file(_pass.c_str(), "r");

	if (!file) {
		printf("公開鍵ファイル読み込みエラー\n");
		return;
	}

	//公開鍵読み込み
	EVP_PKEY* pkay = PEM_read_bio_PUBKEY(file, NULL, NULL, NULL);
	publicKey = EVP_PKEY_get1_RSA(pkay);
	if (!publicKey) {
		printf("公開鍵読み込みエラー");
		return;
	}

	//解放処理
	BIO_free(file);
	EVP_PKEY_free(pkay);

}

int OpenSSLRSA::Encode(char * _encodedata, const char * _originaldata, const int _byte)
{
	char encode[256];						//暗号化データ

	//暗号化
	int outlen = RSA_public_encrypt(_byte, (unsigned char*)_originaldata, (unsigned char*)encode, publicKey, RSA_PKCS1_PADDING);
	memcpy(_encodedata, encode, outlen);																								//暗号データのコピー
	return outlen;
}

int OpenSSLRSA::Decode(char * _decodedata, const char * _encodedata, const int _byte)
{
	char decode[256];						//復号データ

	//復号
	int outlen = RSA_private_decrypt(_byte, (unsigned char*)_encodedata, (unsigned char*)decode, privateKey, RSA_PKCS1_PADDING);
	memcpy(_decodedata, decode, outlen);																								//複合データのコピー
	return outlen;
}
