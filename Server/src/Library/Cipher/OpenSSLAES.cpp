#include"../../Include.h"
#include"OpenSSLAES.h"
#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")



OpenSSLAES::OpenSSLAES()
{
	/*インスタンスの生成*/
	//key = new unsigned char[EVP_MAX_KEY_LENGTH];
	en = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(en);								//EVP_CIPHER_CTXの初期化
}

OpenSSLAES::~OpenSSLAES()
{
	/*解放処理*/
	EVP_CIPHER_CTX_cleanup(en);								//EVP_CIPHER_CTXの削除
	EVP_CIPHER_CTX_free(en);
	//delete key;
}

void OpenSSLAES::CreateKey(const int _keylength)
{
	/*キーの生成*/
	int checkkey=RAND_bytes(key,_keylength);						//鍵のランダム作成
	if (checkkey == 0) {
		checkkey = RAND_bytes(key, EVP_MAX_KEY_LENGTH);			//十分な長さをもつ鍵のランダム作成
	}
}

int OpenSSLAES::Encode(char* _encodedata,const char* _originaldata, const size_t _datalen/*暗号化するデータサイズ*/)
{
	char oridata[128];										//暗号化するデータ
	char endata[128];										//暗号化データ
	int ensize = 0;											//エンコードした分のバイトサイズ
	int paddingsize = 0;									//パディング用のデータが入る
	int outsize=0;											//最終的に出力したデータ量
	int datalen = Multiple16(_datalen);						//16の倍数にそろえる

	memcpy(oridata, _originaldata, _datalen);				//元データのコピー

	/*暗号化処理*/
	EVP_EncryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);												//暗号化の初期化(初期ベクトルは毎回違うものを渡す)AESは=EVP_aes_128_cbc()これを暗号利用モードに設定
	EVP_EncryptUpdate(en, (unsigned char*)&endata, &ensize, (unsigned char*)&oridata,datalen);				//データの暗号化(outlenには16バイトを渡す)
	EVP_EncryptFinal_ex(en, (unsigned char*)&endata + ensize, &paddingsize);								//16の倍数のバイト数になるようにパディングされる
	outsize = ensize + paddingsize;																			//最終的なデータ量の計算
	memcpy(_encodedata,endata, outsize);																	//暗号データのコピー

	return outsize;																							//暗号化処理後のデータ量を返す
}

int OpenSSLAES::Decode(char* _decodedata, const char* _originaldata, const size_t _datalen/*復号するデータサイズ*/)
{
	char oridata[128];									//復号するデータ
	char dedata[128];									//復号したデータ
	int desize=0;										//復号したデータ量
	int outlen=0;										//最終的な出力されたデータ量
	int paddingsize = 0;								//パディングされたデータ量

	memcpy(oridata, _originaldata, _datalen);			//元データのコピー

	/*複合処理*/
	EVP_DecryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);											//復号の初期化
	EVP_DecryptUpdate(en, (unsigned char*)&dedata, &desize, (unsigned char*)&oridata,_datalen);			//復号このままだと_datalenが48の時32でデコードされる
	EVP_DecryptFinal_ex(en, (unsigned char*)&dedata+desize, &paddingsize);								//終了
	outlen = desize + paddingsize;																		//複合処理後のデータ量
	memcpy(_decodedata, dedata, outlen);																//復号データのコピー

	return outlen;																						//複合処理後のデータ量を返す
 }



int OpenSSLAES::Multiple16(int _data)
{
	if (_data % 16 == 0)return _data;
	int remainder = 0;
	for (int i = 1; i < 16; i++) {
		if ((_data + i) % 16 == 0) {
			remainder = i;
			break;
		}
	}
	return _data + remainder;
}
