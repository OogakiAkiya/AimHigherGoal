#include"../../Include.h"
#include"OpenSSLDH.h"


OpenSSLDH::OpenSSLDH()
{
	/*インスタンスの生成*/
	dh_privatekey = DH_new();
}

OpenSSLDH::~OpenSSLDH()
{
	/*解放処理*/
	DH_free(dh_privatekey);
}

void OpenSSLDH::DH_Init(int _keylen)
{
	int code = 0;																		//チェックに使用

	/*鍵交換用のキーの生成*/
	DH_generate_parameters_ex(dh_privatekey, _keylen, DH_GENERATOR_2, NULL);			//生成するキーの設定
	DH_check(dh_privatekey, &code);														//キーに問題がないかチェック
	if (code != 0) {
		if (code & DH_CHECK_P_NOT_PRIME) {
			printf("p value is not prime\n");
		}
		if (code & DH_CHECK_P_NOT_SAFE_PRIME) {
			printf("p value is not a safe prime\n");
		}
		if (code & DH_UNABLE_TO_CHECK_GENERATOR) {
			printf("unable to check the generator value\n");
		}
		if (code & DH_NOT_SUITABLE_GENERATOR) {
			printf("the g value is not a generator\n");
		}
	}
	DH_generate_key(dh_privatekey);														//キーの生成
	char* openkey = BN_bn2hex(dh_privatekey->pub_key);									//BIGNUM型の値をchar型に変換
	char* seacretkey = BN_bn2hex(dh_privatekey->priv_key);								//BIGNUM型の値をchar型に変換
	printf("open=%s\n", openkey);
	printf("sacret=%s\n", seacretkey);

	/*解放処理*/
	OPENSSL_free(openkey);
	OPENSSL_free(seacretkey);


}

void OpenSSLDH::DH_CreateKey(char* _pubkey)
{
	/*共通鍵で使用するキーの生成*/
	BIGNUM* pubkey;
	BN_dec2bn(&pubkey, _pubkey);

	int keylen = DH_size(dh_privatekey);
	unsigned char* keybuf = (unsigned char*)OPENSSL_malloc(keylen);
	int resultlen = DH_compute_key(keybuf,pubkey/*対象のキー*/, dh_privatekey);							//結果がkeybufに入る
	for (int i = 0; i <resultlen; i++) {
		printf("%02X", keybuf[i]);
	}

	OPENSSL_free(keybuf);
	BN_free(pubkey);
}

