#include"../StandardLibraryInclude.h"
#include"Socket.h"
#include"ExtensionMutex.h"
#include"Data.h"
#include"Cipher/OpenSSLAES.h"
#include"Cipher/OpenSSLRSA.h"
#include"Cipher/Cipher.h"
#include"Client.h"

#pragma comment(lib,"Ws2_32.lib")
using namespace std;

Client::Client()
{
	//インスタンスの生成
	cipher = new Cipher();										//暗号処理用クラス
	mutex = new ExtensionMutex();								//排他制御用クラス
	playerData = new Data();
}

Client::~Client()
{
	//切断メッセージ処理
//ヘッダー作成
	Header header;
	header.size = sizeof(Header);
	header.playerIdSize = playerData->GetId()->length();
	memcpy(header.playerId, playerData->GetId()->c_str(), header.playerIdSize);
	header.id = 0xFE;
	//データ送信
	send(GetSocket(), (char*)&header, header.size, 0);

	//解放処理
	if (socket != nullptr)socket->Close();
	if (thread != nullptr)thread->detach();
	thread = nullptr;
	delete mutex;
	delete cipher;
	delete socket;
	delete playerData;
}


void Client::StartThread()
{
	thread = std::make_shared<std::thread>(ClientThreadLauncher,this);
}

bool Client::CreateSocket(std::string _ip)
{
	//既にソケットが存在する場合削除する
	if (socket != nullptr) {
		socket->Close();
		delete socket;
		socket = nullptr;
	}

	//ソケット生成
	socket = Socket::Instantiate()->
		SetProtocolVersion_Dual().
		SetProtocol_TCP().
		SetIpAddress(_ip.c_str()).
		SetPortNumber("49155").
		ClientCreate();
	if (socket == nullptr)return false;
	playerData->SetSocket(socket->GetSocket());

	//Recv処理を行うスレッドを開始させる
	StartThread();

	//鍵交換処理
	ExchangeKey(*playerData->GetId());
	return true;
}

void Client::Recv()
{
	//ローカル変数作成
	int iResult;																						//recv結果が入る(受信したバイト数が入る)
	char rec[RECVBYTESIZE];																				//受け取ったデータを格納

	//受信
	while (1) {
		iResult = socket->Recv(rec, RECVBYTESIZE);
		if (iResult > 0) {

			//受信データを一時データ配列に追加
			int nowSize = tempDataList.size();															//一時データ配列に何byteデータが入っているかを見る
			tempDataList.resize(nowSize + iResult);														//送られてきたデータが格納できるように一時データ配列のサイズ変更
			memcpy((char*)&tempDataList[nowSize], rec, iResult);										//最後尾に送られてきたデータの追加

			//一時データから完全データの作成
			while (tempDataList.size() >= sizeof(int)) {												//何byteのデータが送られてきていいるかすら読み込めなければ抜ける
				//復号処理
				Header recvData = *(Header*)&tempDataList[0];
				if (recvData.size <= (int)tempDataList.size()) {
					int decodeSize = recvData.size - sizeof(Header);
					char data[BYTESIZE];																//復号前データ
					char decodeData[BYTESIZE];															//復号データ
					memcpy(data, &tempDataList[sizeof(Header)], decodeSize);
					tempDataList.erase(tempDataList.begin(), tempDataList.begin() + recvData.size);		//完全データ作成に使用した分を削除
					mutex->Lock();
					int temp = cipher->GetOpenSSLAES()->Decode(decodeData, data, decodeSize);
					mutex->Unlock();
					DataProcess(recvData.id, decodeData);

				}
				else {
					//完全データ作成不能になった場合
					break;
				}
			}

		}
		else if (iResult == 0) {
			//データが送られてこなかった場合切断処理
			break;
		}
		else {
			//エラー処理
			if (WSAGetLastError() != 10035) {									//非同期の場合このエラーが呼び出されることがよくあるが致命的でないためスルーさせて良い
				break;
			}
		}
	}
}

void Client::SendUserInformation(Data * _data)
{
	//データ生成
	Header header;
	header.size = sizeof(Header);
	header.playerIdSize = _data->GetId()->length();
	memcpy(header.playerId, _data->GetId()->c_str(), header.playerIdSize);
	header.id = 0x01;
	send(GetSocket(), (char*)&header, sizeof(Header), 0);
}

void Client::SendPos(Data* _data)
{
	char sendData[BYTESIZE];
	char encodeData[BYTESIZE];

	//データ作成
	PosData data;
	data.x = _data->GetX();
	data.y = _data->GetY();
	data.z = _data->GetZ();
	data.angle = _data->GetAngle();
	data.animation = _data->GetAnimation();

	//暗号処理
	mutex->Lock();
	int encodeSize = cipher->GetOpenSSLAES()->Encode(encodeData, (char*)&data, sizeof(PosData));
	mutex->Unlock();

	//ヘッダー作成
	Header header;
	header.size = sizeof(Header) + encodeSize;
	header.playerIdSize = _data->GetId()->length();
	memcpy(header.playerId, _data->GetId()->c_str(), header.playerIdSize);
	header.id = 0x15;


	//送信データ作成
	memcpy(sendData, &header, sizeof(Header));
	memcpy(&sendData[sizeof(Header)], &encodeData, encodeSize);

	//データ送信
	send(GetSocket(), sendData, header.size, 0);

}

float Client::GetCount()
{
	return count;
}

SOCKET Client::GetSocket()
{
	return socket->GetSocket();
}

Data Client::GetData()
{
	return dataQueueList.front();
}

Data* Client::GetPlayerData()
{
	return playerData;
}

bool Client::GetInitFlg()
{
	return initFlag;
}

void Client::DeleteData()
{
	dataQueueList.pop();
}

void Client::ClearData()
{
	while (1) {
		DeleteData();
		if (DataEmpty() == true)break;
	}
}

bool Client::DataEmpty()
{
	if (dataQueueList.empty() == true)return true;
	return false;
}

void Client::SetEnemyData(int _enemyid, Data * data)
{
	enemyData[_enemyid] = *data;
}

void Client::Lock()
{
	mutex->Lock();
}

void Client::Unlock()
{
	mutex->Unlock();
}

void Client::DataProcess(char _id, char * _data)
{
	Data data;

	switch (_id) {
	case 0x02: {
		Lock();
		playerData->SetX(*(float*)&_data[0]);
		playerData->SetY(*(float*)&_data[sizeof(float) * 1]);
		playerData->SetZ(*(float*)&_data[sizeof(float) * 2]);
		initFlag = true;
		Unlock();
		break;
	}
	 //座標更新処理
	case 0x16: {

		float recvData = *(float*)&_data[0];
		data.SetX(recvData);
		recvData = *(float*)&_data[sizeof(float) * 1];
		data.SetY(recvData);
		recvData = *(float*)&_data[sizeof(float) * 2];
		data.SetZ(recvData);
		recvData = *(float*)&_data[sizeof(float) * 3];
		data.SetAngle(recvData);
		recvData = *(int*)&_data[sizeof(float) * 3 + sizeof(int)];
		data.SetAnimation(recvData);
		break;
	}
	//攻撃処理
	case 0x18:
		data.SetX(0.0f);					//敵の現在の座標
		data.SetY(0.0f);
		data.SetZ(0.0f);
		data.SetAngle(0);
		data.SetAnimation(DAMAGE);
		break;
	}

}

void Client::ExchangeKey(std::string _id)
{
	char keyBuf[EVP_MAX_KEY_LENGTH];														//鍵サイズ
	char endata[BYTESIZE];
	char sendbuf[BYTESIZE];

	//暗号処理
	cipher->GetOpenSSLAES()->GetKey(keyBuf);												//共通鍵取得
	mutex->Lock();
	int outlen = cipher->GetOpenSSLRSA()->Encode(endata, keyBuf, EVP_MAX_KEY_LENGTH);		//暗号化処理
	mutex->Unlock();


	Header sendData;
	sendData.size = sizeof(Header) + outlen;
	sendData.playerIdSize = _id.size();
	memcpy(&sendData.playerId[0], _id.c_str(), sendData.playerIdSize);
	sendData.id = 0xFF;
	memcpy(sendbuf, (char*)&sendData, sizeof(Header));
	memcpy(&sendbuf[sizeof(Header)], (char*)&endata, outlen);

	//送信
	send(socket->GetSocket(), sendbuf, sendData.size, 0);
}

