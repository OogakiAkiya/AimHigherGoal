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
	/*解放処理*/
	thread->join();										//スレッド終了
	delete data;
	delete curl;
	delete thread;
	delete aes;
}



void Client::Recv()
{
	//鍵交換
	while (1) {
		int iResult;										//送られてきたデータ量が格納される
		char rec[BYTESIZE];									//受信データ
		bool exitflg = false;

		/*受信*/
		iResult = recv(data->GetSocket(), rec/*データ*/, sizeof(rec), 0);
		if (iResult > 0) {

			/*受信データを一時データ配列に追加*/
			int now_size = temp_datalist.size();															//一時データ配列に何byteデータが入っているかを見る
			temp_datalist.resize(now_size + iResult);														//送られてきたデータが格納できるように一時データ配列のサイズ変更
			memcpy((char*)&temp_datalist[now_size], rec, iResult);											//最後尾に送られてきたデータの追加

			/*一時データから完全データの作成*/
			while (temp_datalist.size() >= sizeof(int)) {																//何byteのデータが送られてきていいるかすら読み込めなければ抜ける

				
				int decodesize = *(int*)&temp_datalist[0];
				if (decodesize <= (int)temp_datalist.size() - sizeof(int)) {
					char data[BYTESIZE];							//復号前データ
					char decode_data[BYTESIZE];
					memcpy(data, &temp_datalist[sizeof(int)], decodesize);
					int outlen=CIPHER.GetOpenSSLRSA()->Decode(decode_data, data, decodesize);
					aes->SetKey((unsigned char*)decode_data, outlen);
					temp_datalist.erase(temp_datalist.begin(), temp_datalist.begin() + (decodesize + sizeof(int)));	//完全データ作成に使用した分を削除
					exitflg = true;
					break;
				}
				else {
					break;
				}
			}
			if (exitflg)break;
		}
		else if (iResult == 0) {
			/*接続を終了するとき*/
			printf("切断されました\n");
			state = -1;
			return;

		}
		else {
			/*接続エラーが起こった時*/
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			return;

		}

	}

	//ゲーム処理
	while (1) {
		int iResult;										//送られてきたデータ量が格納される
		char rec[BYTESIZE];									//受信データ

		/*受信*/
		iResult = recv(data->GetSocket()/*送信元のソケット*/, rec/*データ*/, sizeof(rec), 0);
		if (iResult > 0) {

			/*受信データを一時データ配列に追加*/
			int now_size = temp_datalist.size();															//一時データ配列に何byteデータが入っているかを見る
			temp_datalist.resize(now_size + iResult);														//送られてきたデータが格納できるように一時データ配列のサイズ変更
			memcpy((char*)&temp_datalist[now_size], rec, iResult);											//最後尾に送られてきたデータの追加

			/*一時データから完全データの作成*/
			while (temp_datalist.size() >= 4) {																//何byteのデータが送られてきていいるかすら読み込めなければ抜ける
				
				/*暗号化されたデータの復号処理*/
				int decodesize = *(int*)&temp_datalist[0];
				if (decodesize <= (int)temp_datalist.size() - 4) {
					char data[BYTESIZE];							//復号前データ
					char decode_data[BYTESIZE];
					memcpy(data, &temp_datalist[sizeof(int)], decodesize);
					aes->Decode(decode_data,data,decodesize);

					/*完全データの生成*/
					int byteSize = *(int*)decode_data;														//4byte分だけ取得しintの値にキャスト
					std::vector<char> compData(byteSize);													//完全データ
					memcpy(&compData[0], &decode_data[sizeof(int)], byteSize);								//サイズ以外のデータを使用し完全データを作成
					MUTEX.Lock();
					completedata_qlist.push(compData);														//完全データ配列に格納
					MUTEX.Unlock();
					temp_datalist.erase(temp_datalist.begin(), temp_datalist.begin() + (decodesize + sizeof(int)));	//完全データ作成に使用した分を削除

				}else {
					break;
				}
			}

		}else if (iResult == 0) {
			/*接続を終了するとき*/
			printf("切断されました\n");
			state = -1;
			break;

		}else {
			/*接続エラーが起こった時*/
			printf("recv failed:%d\n%d", WSAGetLastError(), iResult);
			state = -1;
			break;

		}
	}
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
	return room_number;
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
	room_number = _number;
}


std::vector<char>* Client::GetCompleteData()
{
	return &completedata_qlist.front();
}

void Client::DeleteCompleteData()
{
	completedata_qlist.pop();
}

bool Client::EmptyCompleteData()
{
	if (completedata_qlist.empty() == true) {
		return true;									//空
	}
	return false;										//値が入っている
}

