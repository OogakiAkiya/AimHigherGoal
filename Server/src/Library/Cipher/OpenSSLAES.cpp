#include"../../Include.h"
#include"OpenSSLAES.h"
#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")



OpenSSLAES::OpenSSLAES()
{
	//インスタンスの生成
	key = new unsigned char[EVP_MAX_KEY_LENGTH];
	en = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(en);								//EVP_CIPHER_CTXの初期化
}

OpenSSLAES::~OpenSSLAES()
{
	//解放処理
	EVP_CIPHER_CTX_cleanup(en);								//EVP_CIPHER_CTXの削除
	EVP_CIPHER_CTX_free(en);
	delete key;
}

void OpenSSLAES::CreateKey(const int _keyLength)
{
	//キーの生成
	int checkkey=RAND_bytes(key,_keyLength);						//鍵のランダム作成
	if (checkkey == 0) {
		checkkey = RAND_bytes(key, EVP_MAX_KEY_LENGTH);			//十分な長さをもつ鍵のランダム作成
	}
}

int OpenSSLAES::Encode(char* _encodeData,const char* _originalData, const size_t _dataLen/*暗号化するデータサイズ*/)
{
	char oriData[128];										//暗号化するデータ
	char enData[128];										//暗号化データ
	int encodeSize = 0;										//エンコードした分のバイトサイズ
	int paddingSize = 0;									//パディング用のデータが入る
	int outSize=0;											//最終的に出力したデータ量
	int dataLength = Multiple16(_dataLen);					//16の倍数にそろえる

	memcpy(oriData, _originalData, _dataLen);				//元データのコピー

	//暗号化処理
	EVP_EncryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);												//暗号化の初期化(初期ベクトルは毎回違うものを渡す)AESは=EVP_aes_128_cbc()これを暗号利用モードに設定
	EVP_EncryptUpdate(en, (unsigned char*)&enData, &encodeSize, (unsigned char*)&oriData,dataLength);		//データの暗号化(outlenには16バイトを渡す)
	EVP_EncryptFinal_ex(en, (unsigned char*)&enData + encodeSize, &paddingSize);							//16の倍数のバイト数になるようにパディングされる
	outSize = encodeSize + paddingSize;																		//最終的なデータ量の計算
	memcpy(_encodeData,enData, outSize);																	//暗号データのコピー

	return outSize;																							//暗号化処理後のデータ量を返す
}

int OpenSSLAES::Decode(char* _decodeData, const char* _originalData, const size_t _dataLen/*復号するデータサイズ*/)
{
	char oriData[128];									//復号するデータ
	char deData[128];									//復号したデータ
	int deSize=0;										//復号したデータ量
	int outLength=0;									//最終的な出力されたデータ量
	int paddingSize = 0;								//パディングされたデータ量

	memcpy(oriData, _originalData, _dataLen);			//元データのコピー

	//復号処理
	EVP_DecryptInit_ex(en, EVP_aes_128_cbc(), NULL, key, iv);											//復号の初期化
	EVP_DecryptUpdate(en, (unsigned char*)&deData, &deSize, (unsigned char*)&oriData,_dataLen);			//復号このままだと_datalenが48の時32でデコードされる
	EVP_DecryptFinal_ex(en, (unsigned char*)&deData+deSize, &paddingSize);								//終了
	outLength = deSize + paddingSize;																	//複合処理後のデータ量
	memcpy(_decodeData, deData, outLength);																//復号データのコピー

	return outLength;																					//複合処理後のデータ量を返す
 }

void OpenSSLAES::SetKey(unsigned char * _key,int _keyLength)
{
	//引数の値を鍵に設定する
	memcpy(key, _key, _keyLength);
	printf("共通鍵=%s\n", key);
}

void OpenSSLAES::GetKey(char * _key)
{
	memcpy(_key, key, EVP_MAX_KEY_LENGTH);
}

int OpenSSLAES::Multiple16(int _data)
{
	//16の倍数の場合そのまま値を返す
	if (_data % 16 == 0)return _data;

	//16の倍数で値を返す
	int remainder = 0;
	for (int i = 1; i < 16; i++) {
		if ((_data + i) % 16 == 0) {
			remainder = i;
			break;
		}
	}
	return _data + remainder;
}
