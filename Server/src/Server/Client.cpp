#include"../Include.h"
#include"../Library/Data/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"../Library/CurlWrapper/CurlWrapper.h"
#include"../Library/Socket/Socket.h"
#include "Client.h"

Client::Client()
{
	completeDataQueList = std::make_unique <std::queue<std::vector<char>>>();
	data = std::make_shared<Data>();
	curl = std::make_unique<CurlWrapper>();
	aes = std::make_unique<OpenSSLAES>();
}

Client::~Client()
{
	//解放処理
	if(thread!=nullptr)thread->detach();											//スレッド終了
	thread = nullptr;
	curl = nullptr;
	aes=nullptr;
	completeDataQueList = nullptr;
	data = nullptr;

}

void Client::Update()
{
	//鍵交換
	if (!keyChangeFlg) {
		RecvLoop(EXCHANGEKEY);
	}
	//完全データ作成処理
	if (keyChangeFlg) {
		RecvLoop(CREATECOMPLETEDATA);
	}
	if (posGetFlg) {
		//curl->PosUpdataLoop(data);				//スレッドを使わない場合重くなる
	}
}


SOCKET Client::GetSocket()
{
	return data->GetSocket();
}

int Client::GetRoomNumber()
{
	return roomNumber;
}

std::shared_ptr<Data> Client::GetData()
{
	return data;
}

int Client::GetState()
{
	return state;
}

OpenSSLAES* Client::GetAES()
{
	return aes.get();
}

CurlWrapper* Client::GetCurl()
{
	return curl.get();
}


void Client::SetSocket(SOCKET _socket)
{
	data->SetSocket(_socket);
}

void Client::SetNumber(int _number)
{
	roomNumber = _number;
}

void Client::SetPosGetFlg()
{
	posGetFlg = true;
}

bool Client::GetPosGetFlg()
{
	return posGetFlg;
}

std::vector<char>* Client::GetCompleteData()
{
	return &completeDataQueList->front();
}

void Client::DeleteCompleteData()
{
	completeDataQueList->pop();
}

bool Client::EmptyCompleteData()
{
	if (completeDataQueList->empty() == true) {
		return true;									//空
	}
	return false;										//値が入っている
}

void Client::RecvLoop(int _loopType)
{
		int iResult;										//送られてきたデータ量が格納される
		char rec[BYTESIZE*2];								//受信データ

		//受信
		iResult = recv(data->GetSocket(), rec, sizeof(rec), 0);
		if (iResult > 0) {
			//受信データを一時データ配列に追加
			int nowSize = tempDataList.size();										//一時データ配列に何byteデータが入っているかを見る
			tempDataList.resize(nowSize + iResult);									//送られてきたデータが格納できるように一時データ配列のサイズ変更
			memcpy((char*)&tempDataList[nowSize], rec, iResult);					//最後尾に送られてきたデータの追加

			//鍵交換
			if (_loopType == EXCHANGEKEY) {
				if (ExchangeKey() == true)return;
			}
			//完全データ作成処理
			if (_loopType == CREATECOMPLETEDATA)CreateCompleteData();

		}
		else if (iResult == 0) {
			//接続を終了するとき
			printf("切断されました\n");
			state = -1;
			return;
		}
		else if (WSAGetLastError() == 10035) {
			//非同期の場合clientがsendしていなかったときにおこるエラー
			return;
		}
		else {
			//接続エラーが起こった時
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			return;
		}
}

bool Client::ExchangeKey()
{
	while (tempDataList.size() >= sizeof(int)) {
		//復号に十分なデータがあるかチェック
		int decodeSize = *(int*)&tempDataList[0];
		if (decodeSize <= (int)tempDataList.size() - sizeof(int)) {

			//変数宣言
			char data[BYTESIZE];																			//復号前データ
			char decodeData[BYTESIZE];

			//復号処理
			memcpy(data, &tempDataList[sizeof(int)], decodeSize);											//復号するデータのコピー
			int outLen = CIPHER.GetOpenSSLRSA()->Decode(decodeData, data, decodeSize);						//公開鍵暗号の復号
			aes->SetKey((unsigned char*)decodeData, outLen);												//共通鍵を設定
			tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + sizeof(int)));	//完全データ作成に使用した分を削除
			keyChangeFlg = true;
			return true;
		}
		else {
			return false;
		}
	}
}

void Client::CreateCompleteData()
{
	while (tempDataList.size() >= sizeof(int)) {
		//復号に十分なデータがあるかチェック
		int decodeSize = *(int*)&tempDataList[0];
		if (decodeSize <= (int)tempDataList.size() - sizeof(int)) {

			//変数宣言
			char data[BYTESIZE];																			//復号前データ
			char decodeData[BYTESIZE];
			//復号処理
			memcpy(data, &tempDataList[sizeof(int)], decodeSize);
			aes->Decode(decodeData, data, decodeSize);

			//完全データの生成
			int byteSize = *(int*)decodeData;																	//4byte分だけ取得しintの値にキャスト
			if (byteSize < BYTESIZE&&byteSize>0) {
				std::vector<char> compData(byteSize);															//完全データ
				memcpy(&compData[0], &decodeData[sizeof(int)], byteSize);										//サイズ以外のデータを使用し完全データを作成
				completeDataQueList->push(compData);																//完全データ配列に格納
			}
			tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + sizeof(int)));		//完全データ作成に使用した分を削除

		}
		else {
			break;
		}
	}
}