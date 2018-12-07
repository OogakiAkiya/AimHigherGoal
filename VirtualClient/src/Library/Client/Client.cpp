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
	//thread = new std::thread(ClientThreadLauncher, (void*)this);
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
	ExchangeKey();
	return true;
}

void Client::Recv()
{
	//ローカル変数作成
	int iResult;																							//recv結果が入る(受信したバイト数が入る)
	char rec[BYTESIZE * 2];																					//受け取ったデータを格納

	//受信
	while (1) {
		iResult = socket->Recv(rec, BYTESIZE);
		if (iResult > 0) {

			//受信データを一時データ配列に追加
			int nowSize = tempDataList.size();																//一時データ配列に何byteデータが入っているかを見る
			tempDataList.resize(nowSize + iResult);															//送られてきたデータが格納できるように一時データ配列のサイズ変更
			memcpy((char*)&tempDataList[nowSize], rec, iResult);											//最後尾に送られてきたデータの追加

			//一時データから完全データの作成
			while (tempDataList.size() >= sizeof(int)) {													//何byteのデータが送られてきていいるかすら読み込めなければ抜ける
				try {
					//復号処理
					int decodeSize = *(int*)&tempDataList[0];
					if (decodeSize <= (int)tempDataList.size() - sizeof(int)) {
						char data[BYTESIZE];																	//復号前データ
						char decodeData[BYTESIZE];																//復号データ
						memcpy(data, &tempDataList[sizeof(int)], decodeSize);
						cipher->GetOpenSSLAES()->Decode(decodeData, data, decodeSize);
						int byteSize = *(int*)decodeData;														//4byte分だけ取得しintの値にキャスト
						std::vector<char> compData(byteSize);													//完全データ
						memcpy(&compData[0], &decodeData[sizeof(int)], byteSize);								//サイズ以外のデータを使用し完全データを作成
						tempDataList.erase(tempDataList.begin(), tempDataList.begin() + (decodeSize + 4));		//完全データ作成に使用した分を削除

						//完全データの処理
						DataManipulate(&compData);
					}

					else {
						//完全データ作成不能になった場合
						break;
					}
				}
				catch (std::exception error) {
					printf("%s", error.what());
				}
			}
		}
		else if (iResult == 0) {
			//データが送られてこなかった場合切断処理
			break;
		}
		else {
			//エラー処理
			int error = WSAGetLastError();
			if (error != 10035) {									//非同期の場合このエラーが呼び出されることがよくあるが致命的でないためスルーさせて良い
				break;
			}
		}
	}
}

void Client::SendUserInformation(Data * _data)
{
	//データ生成
	UserData sendData;
	sendData.base.id = 0x01;
	sendData.idsize = _data->GetId()->length();
	memcpy(&sendData.id[0], _data->GetId()->c_str(), sendData.idsize);
	sendData.base.size = sizeof(UserData) - sizeof(int);

	char* origin = (char*)&sendData;
	char encodeData[BYTESIZE];								//暗号化データを入れる
	char senData[BYTESIZE];									//送信データ

	//暗号処理
	int encode_size = cipher->GetOpenSSLAES()->Encode(encodeData, origin,sendData.base.size+sizeof(UserData));
	memcpy(senData, &encode_size, sizeof(int));
	memcpy(&senData[sizeof(int)], encodeData, encode_size);

	//データ送信
	send(socket->GetSocket() , senData, sizeof(int) + encode_size, 0);

}

void Client::SendPos(Data* _data)
{
	//送信データの生成
	PosData data;
	data.base.size = sizeof(PosData) - sizeof(int);
	data.base.id = 0x15;
	data.x = _data->GetX();
	data.y = _data->GetY();
	data.z = _data->GetZ();
	data.angle = _data->GetAngle();
	data.animation = _data->GetAnimation();

	//変数生成
	char* origin = (char*)&data;
	char encodeData[BYTESIZE];										//暗号化データを入れる
	char sendData[BYTESIZE];										//送信データ

	//暗号処理
	int encode_size = cipher->GetOpenSSLAES()->Encode(encodeData, origin, sizeof(PosData));
	memcpy(sendData, &encode_size, sizeof(int));
	memcpy(&sendData[sizeof(int)], encodeData, encode_size);

	//データ送信
	send(socket->GetSocket(), sendData, sizeof(int) + encode_size, 0);
}

void Client::SendAttack(Data* _data)
{
	//当たったかどうかを判定
	float length;
	D3DXVECTOR3 vectorLength;
	D3DXVECTOR3 playerVector(_data->GetX(), _data->GetY(), _data->GetZ());											//プレイヤーのベクトル
	for (int element = 0; element < 3; element++) {																	//idは敵の区別
		D3DXVECTOR3 enemyVector(enemyData[element].GetX(), enemyData[element].GetY(), enemyData[element].GetZ());	//敵のベクトル
		vectorLength = enemyVector - playerVector;																	//二つのベクトルの差
		length = D3DXVec3Length(&vectorLength);

		//データ送信(当たった場合)
		if (length <= 2.0f) {
			char encodeData[BYTESIZE];					//暗号データ
			char sendData[BYTESIZE];					//送信データ

			//データの生成
			AttckData data;
			data.base.size = sizeof(AttckData) - sizeof(int);
			data.base.id = 0x17;
			data.socket = element;
			
			//暗号処理
			int encodeSize = cipher->GetOpenSSLAES()->Encode(encodeData, (char*)&data, sizeof(AttckData));
			memcpy(sendData, &encodeSize, sizeof(int));
			memcpy(&sendData[sizeof(int)], encodeData, encodeSize);

			//送信処理
			send(socket->GetSocket(), (char*)&sendData, sizeof(int) + encodeSize, 0);
		}
	}
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

void Client::DataManipulate(const std::vector<char>* _data)
{
	Data data;
	char id = *(char*)&_data->at(0);					//なんのデータかidで判断する

	switch (id) {
	case 0x02: {
		Lock();
		playerData->SetX(*(float*)&_data->at(sizeof(char)));
		playerData->SetY(*(float*)&_data->at(sizeof(char) + sizeof(float) * 1));
		playerData->SetZ(*(float*)&_data->at(sizeof(char) + sizeof(float) * 2));
		initFlag = true;
		Unlock();
		break;
	}

	//座標更新処理
	case 0x16: {
		float recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 0);
		data.SetX(recvData);
		recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 1);
		data.SetY(recvData);
		recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 2);
		data.SetZ(recvData);
		recvData = *(float*)&_data->at(sizeof(char) + sizeof(float) * 3);
		data.SetAngle(recvData);
		recvData = *(int*)&_data->at(sizeof(char) + sizeof(float) * 3 + sizeof(int));
		data.SetAnimation(recvData);
		Lock();
		dataQueueList.push(data);
		Unlock();
		break;
	}

	//攻撃処理
	case 0x18:
		data.SetX(0.0f);					//敵の現在の座標
		data.SetY(0.0f);
		data.SetZ(0.0f);
		data.SetAngle(0);
		data.SetAnimation(DAMAGE);
		Lock();
		dataQueueList.push(data);
		Unlock();
		break;
	}

}

void Client::ExchangeKey()
{
	//変数生成
	char keyBuf[EVP_MAX_KEY_LENGTH];							//鍵サイズ
	char endata[BYTESIZE];
	char sendbuf[BYTESIZE];

	//暗号処理
	cipher->GetOpenSSLAES()->GetKey(keyBuf);												//共通鍵取得
	int outlen = cipher->GetOpenSSLRSA()->Encode(endata, keyBuf, EVP_MAX_KEY_LENGTH);		//暗号化処理
	memcpy(sendbuf, &outlen, sizeof(int));
	memcpy(&sendbuf[sizeof(int)], &endata, outlen);
	
	//送信
	send(socket->GetSocket(), sendbuf, sizeof(int) + outlen, 0);

}

