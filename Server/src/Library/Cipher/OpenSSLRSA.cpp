#include"../../Include.h"
#include"OpenSSLRSA.h"

OpenSSLRSA::OpenSSLRSA()
{
	BIO* bio = BIO_new_mem_buf(key.c_str(),key.size());
	privateKey = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);

	BIO_free(bio);

}

OpenSSLRSA::~OpenSSLRSA()
{
	RSA_free(privateKey);
}

void OpenSSLRSA::CreateKey(const std::string _private_key_file, const std::string _public_key_file)
{

	BIO* bi = BIO_new_mem_buf(pubKey.c_str(),pubKey.size());
	if (bi == NULL) {
		return;
	}
	EVP_PKEY* pkay= PEM_read_bio_PUBKEY(bi, NULL, NULL, NULL);
	RSA* publicKey = EVP_PKEY_get1_RSA(pkay);
	if (publicKey == NULL) {
		printf("NULL");
		return;
	}

	int privateSize = RSA_size(privateKey);
	int publicSize = RSA_size(publicKey);

	printf("privateKeysize=%d,publicKeysize=%d\n", privateSize, publicSize);

	char encodedata[512];
	char decodedata[512];
	std::string originaldata = "オリジナルデータです。よろしくお願いします。Hello";

	int outlen = RSA_public_encrypt(originaldata.size(), (unsigned char*)originaldata.c_str(), (unsigned char*)encodedata, publicKey, RSA_PKCS1_PADDING);
	printf("\n encode=%s\n", encodedata);

	int decodelen = RSA_private_decrypt(outlen, (unsigned char*)&encodedata[0], (unsigned char*)&decodedata[0], privateKey, RSA_PKCS1_PADDING);
	printf("\n decode=%s\n",decodedata);
	BIO_free(bi);
	EVP_PKEY_free(pkay);
	RSA_free(publicKey);


	/*
		FILE *privateKeyFile;
		FILE *publicKeyFile;
		if (fopen_s(&privateKeyFile, _private_key_file.c_str(), "w") != 0) {
			return;
		}
		if (fopen_s(&publicKeyFile, _public_key_file.c_str(), "w") !=0 ) {
			return;
		}

		RSA* rsaKey=RSA_generate_key(size, expornent, NULL, NULL);				//キーペアの作成
		if (rsaKey == NULL) {
			fclose(privateKeyFile);
			fclose(publicKeyFile);
			return;
		}

		//公開鍵の出力
		int test;
		if (PEM_write_RSAPublicKey(publicKeyFile, rsaKey)!=1) {
			printf("PrintErrorPublicKey");
			RSA_free(rsaKey);													//キーペアの解放
			fclose(privateKeyFile);
			fclose(publicKeyFile);
			return;
		}

		//秘密鍵の出力
		if (PEM_write_RSAPrivateKey(privateKeyFile, rsaKey, NULL, NULL, 0, NULL, NULL) != 1) {
			printf("PrintErrorPrivateKey");
			RSA_free(rsaKey);													//キーペアの解放
			fclose(privateKeyFile);
			fclose(publicKeyFile);
			return;

		}

		RSA_free(rsaKey);													//キーペアの解放
		fclose(privateKeyFile);
		fclose(publicKeyFile);
		return;
		*/
		/*
		RSA *rsa;
		FILE *fp=nullptr;
		int rsaSize;
		char *enc;
		char planeData[] = "abcdefghijklmnopqrstuvwxyz";
		int error;


		if ((error=fopen_s(&fp,"key/public-key.pem","r"))!=0) {
			printf("公開鍵オープンエラー\n");
			return;
		}
		rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);

		if ((rsa = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL)) == NULL) {
			printf("公開鍵読み込みエラー\n");
			fclose(fp);
			return;
		}

		fclose(fp);
	*/
	/*
	BIO* bio = BIO_new_mem_buf(key, (int)sizeof(key));
	//EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
	RSA* rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);

	//printf("%s", pkey);
	BIO_free(bio);
	RSA_free(rsa);
	*/
}
