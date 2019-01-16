#include"../Include.h"
#include"../Library/Data/Data.h"
#include"../Library/NamedPipe/NamedPipe.h"
#include"../Library/Socket/Socket.h"
#include "Client.h"

Client::Client()
{
	pipeDataList = std::make_shared<std::queue<NamedPipe::PipeData>>();
	data = std::make_shared<Data>();
}

Client::~Client()
{
	//解放処理
	pipeDataList = nullptr;
	data = nullptr;

}

void Client::Update()
{
	RecvLoop();
}


SOCKET Client::GetSocket()
{
	return data->GetSocket();
}

std::shared_ptr<Data> Client::GetData()
{
	return data;
}

int Client::GetState()
{
	return state;
}

NamedPipe::PipeData* Client::GetPipeData()
{
	return &pipeDataList->front();
}

void Client::DeletePipeData()
{
	pipeDataList->pop();
}

bool Client::EmptyPipeData()
{
	if (pipeDataList->empty() == true) {
		return true;									//空
	}
	return false;										//値が入っている
}

void Client::RecvLoop()
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

			while (tempDataList.size() >= sizeof(int)) {
				//パイプに送るデータがあるかチェック
				int oneDataSize = *(int*)&tempDataList[0];
				if (oneDataSize <= (int)tempDataList.size()) {
					//パイプに送るデータの作成
					NamedPipe::PipeData pipeData;
					pipeData.byteSize=oneDataSize;
					memcpy(&pipeData.data, &tempDataList[0], oneDataSize);
					pipeDataList->push(pipeData);
					tempDataList.erase(tempDataList.begin(), tempDataList.begin() + pipeData.byteSize);
				}
			}
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
