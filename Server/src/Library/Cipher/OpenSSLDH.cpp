#include"../../Include.h"
#include"OpenSSLDH.h"


OpenSSLDH::OpenSSLDH()
{
	/*�C���X�^���X�̐���*/
	dh_privatekey = DH_new();
}

OpenSSLDH::~OpenSSLDH()
{
	/*�������*/
	DH_free(dh_privatekey);
}

void OpenSSLDH::DH_Init(int _keylen)
{
	int code = 0;																		//�`�F�b�N�Ɏg�p

	/*�������p�̃L�[�̐���*/
	DH_generate_parameters_ex(dh_privatekey, _keylen, DH_GENERATOR_2, NULL);			//��������L�[�̐ݒ�
	DH_check(dh_privatekey, &code);														//�L�[�ɖ�肪�Ȃ����`�F�b�N
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
	DH_generate_key(dh_privatekey);														//�L�[�̐���
	char* openkey = BN_bn2hex(dh_privatekey->pub_key);									//BIGNUM�^�̒l��char�^�ɕϊ�
	char* seacretkey = BN_bn2hex(dh_privatekey->priv_key);								//BIGNUM�^�̒l��char�^�ɕϊ�
	printf("open=%s\n", openkey);
	printf("sacret=%s\n", seacretkey);

	/*�������*/
	OPENSSL_free(openkey);
	OPENSSL_free(seacretkey);


}

void OpenSSLDH::DH_CreateKey(char* _pubkey)
{
	/*���ʌ��Ŏg�p����L�[�̐���*/
	BIGNUM* pubkey;
	BN_dec2bn(&pubkey, _pubkey);

	int keylen = DH_size(dh_privatekey);
	unsigned char* keybuf = (unsigned char*)OPENSSL_malloc(keylen);
	int resultlen = DH_compute_key(keybuf,pubkey/*�Ώۂ̃L�[*/, dh_privatekey);							//���ʂ�keybuf�ɓ���
	for (int i = 0; i <resultlen; i++) {
		printf("%02X", keybuf[i]);
	}

	OPENSSL_free(keybuf);
	BN_free(pubkey);
}

