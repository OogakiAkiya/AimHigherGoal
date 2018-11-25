#include"../Include.h"
#include"../Library/ExtensionMutex.h"
#include"../Library/Data.h"
#include"../Library/Cipher/OpenSSLAES.h"
#include"../Library/Cipher/OpenSSLDH.h"
#include"../Library/Cipher/OpenSSLRSA.h"
#include"../Library/Cipher/Cipher.h"
#include"CurlWrapper.h"
#include"Socket.h"
#include "Client.h"

Client::Client()
{
	data = new Data();
	curl = new CurlWrapper();
	aes = new OpenSSLAES();
}

Client::~Client()
{
	//解放処理
	thread->join();											//スレッド終了
	delete data;
	delete curl;
	delete thread;
	delete aes;
}

void Client::Recv()
{
	//鍵交換
	RecvLoop(EXCHANGEKEY);
	
	//完全データ作成処理
	RecvLoop(CREATECOMPLETEDATA);
}

void Client::StartRecvThread(Client* _client)
{
	thread = new std::thread(RecvLauncher, (void*)_client);
}

void Client::StartHttpThread()
{
	curl->StartThread(curl, data);
}

SOCKET Client::GetSocket()
{
	return data->GetSocket();
}

int Client::GetRoomNumber()
{
	return roomNumber;
}

Data* Client::GetData()
{
	return data;
}

int Client::GetState()
{
	return state;
}

OpenSSLAES * Client::GetAES()
{
	return aes;
}


void Client::SetSocket(SOCKET _socket)
{
	data->SetSocket(_socket);
}

void Client::SetNumber(int _number)
{
	roomNumber = _number;
}

std::vector<char>* Client::GetCompleteData()
{
	return &completeDataQueList.front();
}

void Client::DeleteCompleteData()
{
	completeDataQueList.pop();
}

bool Client::EmptyCompleteData()
{
	if (completeDataQueList.empty() == true) {
		return true;									//空
	}
	return false;										//値が入っている
}

void Client::RecvLoop(int _loopType)
{
	while (1) {
		int iResult;										//送られてきたデータ量が格納される
		char rec[BYTESIZE];									//受信データ

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
		else {
			//接続エラーが起こった時
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			return;
		}
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
		if (decodeSize <= (int)tempDataList.size() - 4) {

			//変数宣言
			char data[BYTESIZE];																			//復号前データ
			char decodeData[BYTESIZE];

			//復号処理
			memcpy(data, &tempDataList[sizeof(int)], decodeSize);
			aes->Decode(decodeData, data, decodeSize);

			//完全データの生成
			int byteSize = *(int*)decodeData;																//4byte分だけ取得しintの値にキャスト
			std::vector<char> compData(byteSize);															//完全データ
			memcpy(&compData[0], &decodeData[sizeof(int)], byteSize);										//サイズ以外のデータを使用し完全データを作成
			MUTEX.Lock();																					//排他制御開始
			completeDataQueList.push(compData);																//完全データ配列に格納
			MUTEX.Unlock();																					//排他制御終了
			tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + sizeof(int)));	//完全データ作成に使用した分を削除
		}
		else {
			break;
		}
	}
}