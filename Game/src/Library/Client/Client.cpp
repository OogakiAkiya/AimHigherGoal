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
Client* Client::s_Instance = nullptr;

Client::Client()
{
	//インスタンスの生成
	cipher = std::make_unique<Cipher>();
	mutex = std::make_unique<ExtensionMutex>();
	playerData = std::make_unique<Data>();
	for (int i = 0; i < ENEMYAMOUNT; i++) {
		enemyData[i] = std::make_shared<Data>();				//プレイヤー以外の敵情報を保持
	}
	dataQueueList = std::make_shared<std::queue<Data>>();		//完成データから作成された各情報を保持

}

Client::~Client()
{
	//切断メッセージ処理
	//ヘッダー作成
	if (socket!=nullptr) {
		Header header;
		header.size = sizeof(Header);
		header.playerIdSize = playerData->GetId()->length();
		memcpy(header.playerId, playerData->GetId()->c_str(), header.playerIdSize);
		header.id = 0xFE;
		//データ送信
		send(CLIENT.GetSocket(), (char*)&header, header.size, 0);
	}

	//解放処理
	if(thread!=nullptr)thread->detach();
	cipher = nullptr;
	mutex = nullptr;
	socket = nullptr;
	playerData = nullptr;
	tempDataList.clear();
	thread = nullptr;
	for (int i = 0; i < ENEMYAMOUNT; i++) {
		enemyData[i] = nullptr;									//プレイヤー以外の敵情報を保持
	}
	dataQueueList = nullptr;									//完成データから作成された各情報を保持
}


void Client::StartThread()
{
	thread = std::make_unique<std::thread>(ClientThreadLauncher, (void*)s_Instance);

}

bool Client::CreateSocket(std::string _userId,std::string _ip)
{
	//既にソケットが存在する場合削除する
	if (socket != nullptr) {
		socket = nullptr;
	}
	if (thread != nullptr) {
		thread->detach();
		thread = nullptr;
	}
	//ソケット生成
	socket = std::make_shared<Socket>();
	socket = socket->
		SetProtocolVersion_Dual().
		SetProtocol_TCP().
		SetIpAddress(_ip.c_str()).
		SetPortNumber("49155").
		SetAsynchronous().
		ClientCreate(socket);

	if (socket == nullptr)return false;

	//Recv処理を行うスレッドを開始させる
	StartThread();

	//鍵交換処理
	ExchangeKey(_userId);
	return true;
}

void Client::Recv()
{
	//ローカル変数作成
	int iResult;																						//recv結果が入る(受信したバイト数が入る)
	char rec[BYTESIZE*2];																				//受け取ったデータを格納
	
	//受信
	while (1) {
		iResult = socket->Recv(rec,BYTESIZE);
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
					int temp=cipher->GetOpenSSLAES()->Decode(decodeData,data,decodeSize);
					mutex->Unlock();

					//自分の情報だけ取得する
					std::string targetName=std::string(recvData.playerId);
					targetName.resize(recvData.playerIdSize);
					if (targetName == *playerData->GetId()) {
						DataProcess(recvData.id, decodeData);
					}
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
	send(CLIENT.GetSocket(), (char*)&header,sizeof(Header), 0);

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
	memcpy(&sendData[sizeof(Header)], &encodeData,encodeSize);

	//データ送信
	send(CLIENT.GetSocket(), sendData,header.size, 0);

}

void Client::SendAttack(std::shared_ptr<Data> _data)
{
	//当たったかどうかを判定
	float length;
	D3DXVECTOR3 vectorLength;
	D3DXVECTOR3 playerVector(_data->GetX(), _data->GetY(), _data->GetZ());												//プレイヤーのベクトル
	for (int element = 0; element < ENEMYAMOUNT; element++) {															//idは敵の区別
		D3DXVECTOR3 enemyVector(enemyData[element]->GetX(), enemyData[element]->GetY(), enemyData[element]->GetZ());	//敵のベクトル
		vectorLength = enemyVector - playerVector;																		//二つのベクトルの差
		length = D3DXVec3Length(&vectorLength);

		//データ送信(当たった場合)
		if (length <= 2.0f) {

			char sendData[BYTESIZE];
			char encodeData[BYTESIZE];

			//データ作成
			int data = element;

			//暗号処理
			mutex->Lock();
			int encodeSize = cipher->GetOpenSSLAES()->Encode(encodeData, (char*)&data, sizeof(PosData));
			mutex->Unlock();

			//ヘッダー作成
			Header header;
			header.size = sizeof(Header) + encodeSize;
			header.playerIdSize = _data->GetId()->length();
			memcpy(header.playerId, _data->GetId()->c_str(), header.playerIdSize);
			header.id = 0x17;


			//送信データ作成
			memcpy(sendData, &header, sizeof(Header));
			memcpy(&sendData[sizeof(Header)], &encodeData, encodeSize);

			//データ送信
			send(CLIENT.GetSocket(), sendData, header.size, 0);

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
	return dataQueueList->front();
}

Data* Client::GetPlayerData()
{
	return playerData.get();
}

bool Client::GetInitFlg()
{
	return initFlag;
}

void Client::DeleteData()
{
	dataQueueList->pop();
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
	if (dataQueueList->empty() == true)return true;
	return false;
}

void Client::SetEnemyData(int _enemyid, std::shared_ptr<Data> data)
{
	enemyData[_enemyid]= data;
}

void Client::Lock()
{
	mutex->Lock();
}

void Client::Unlock()
{
	mutex->Unlock();
}

Client & Client::GetInstance()
{
	if (s_Instance == nullptr) { s_Instance = new Client(); }
	return *s_Instance;
}

void Client::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}
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
		Lock();
		dataQueueList->push(data);
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
		dataQueueList->push(data);
		Unlock();
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
	send(socket->GetSocket(), sendbuf,sendData.size, 0);

}

