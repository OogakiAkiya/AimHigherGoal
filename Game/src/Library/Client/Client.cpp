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
	/*インスタンスの生成*/
	cipher = new Cipher();										//暗号処理用クラス
	mutex = new ExtensionMutex();								//排他制御用クラス
	socket = Socket::Instantiate()->
		SetProtocolVersion_Dual().
		SetProtocol_TCP().
		SetIpAddress("localhost").
		SetPortNumber("49155").
		ClientCreate();
	
	char keyBuf[EVP_MAX_KEY_LENGTH];
	char endata[BYTESIZE];
	char sendbuf[BYTESIZE];
	cipher->GetOpenSSLAES()->GetKey(keyBuf);
	int outlen=cipher->GetOpenSSLRSA()->Encode(endata, keyBuf, EVP_MAX_KEY_LENGTH);
	memcpy(sendbuf, &outlen, sizeof(int));
	memcpy(&sendbuf[sizeof(int)], &endata, outlen);
	send(socket->GetSocket(), sendbuf,sizeof(int)+outlen,0);
}

Client::~Client()
{
	/*解放処理*/
	socket->Close();
	thread->detach();
	delete mutex;
	delete cipher;
	delete socket;
}


void Client::StartThread()
{
	thread = new std::thread(ClientThreadLauncher, (void*)s_Instance);
}

void Client::Recv()
{
	/*変数作成*/
	int iResult;																							//recv結果が入る(受信したバイト数が入る)
	char rec[BYTESIZE];																							//受け取ったデータを格納
	
	/*受信*/
	while (1) {
		iResult = socket->Recv(rec,BYTESIZE);
		if (iResult > 0) {

			/*受信データを一時データ配列に追加*/
			int now_size = temp_datalist.size();															//一時データ配列に何byteデータが入っているかを見る
			temp_datalist.resize(now_size + iResult);														//送られてきたデータが格納できるように一時データ配列のサイズ変更
			memcpy((char*)&temp_datalist[now_size], rec, iResult);											//最後尾に送られてきたデータの追加

			/*一時データから完全データの作成*/
			while (temp_datalist.size() >= 4) {																//何byteのデータが送られてきていいるかすら読み込めなければ抜ける
				
				/*復号処理*/
				int decodesize = *(int*)&temp_datalist[0];
				if (decodesize <= (int)temp_datalist.size() - 4) {
					char data[BYTESIZE];																	//復号前データ
					char decode_data[BYTESIZE];																//復号データ
					memcpy(data, &temp_datalist[sizeof(int)], decodesize);
					cipher->GetOpenSSLAES()->Decode(decode_data, data, decodesize);
					int byteSize = *(int*)decode_data;														//4byte分だけ取得しintの値にキャスト
					std::vector<char> compData(byteSize);													//完全データ
					memcpy(&compData[0], &decode_data[sizeof(int)], byteSize);								//サイズ以外のデータを使用し完全データを作成
					temp_datalist.erase(temp_datalist.begin(), temp_datalist.begin() + (decodesize + 4));	//完全データ作成に使用した分を削除

					/*完全データの処理*/
					DataManipulate(&compData);
				}
				else {
					/*完全データ作成不能になった場合*/
					break;
				}
			}

		}
		else if (iResult == 0) {
			/*データが送られてこなかった場合切断処理*/
			break;
		}
		else {
			/*エラー処理*/
			int error = WSAGetLastError();
			if (error != 10035) {									//非同期の場合このエラーが呼び出されることがよくあるが致命的でないためスルーさせて良い
				break;
			}
		}
	}
}

void Client::SendPos(Data* _data)
{
	/*送信データの生成*/
	PosData data;
	data.size = sizeof(PosData) - sizeof(int);
	data.id = 0x15;
	data.x = _data->GetX();
	data.y = _data->GetY();
	data.z = _data->GetZ();
	data.angle = _data->GetAngle();
	data.animation = _data->GetAnimation();

	/*暗号処理*/
	char* origin = (char*)&data;
	char encode[BYTESIZE];										//暗号化データを入れる
	char senddata[BYTESIZE];									//送信データ
	int encode_size = cipher->GetOpenSSLAES()->Encode(encode, origin, sizeof(PosData));
	memcpy(senddata, &encode_size, sizeof(int));
	memcpy(&senddata[sizeof(int)], encode, encode_size);

	/*データ送信*/
	send(CLIENT.GetSocket(), senddata, sizeof(int) + encode_size, 0);
}

void Client::SendAttack(Data* _data)
{
	/*当たったかどうかを判定*/
	float Lenght;
	D3DXVECTOR3 VLenght;
	D3DXVECTOR3 player_vector(_data->GetX(), _data->GetY(), _data->GetZ());													//プレイヤーのベクトル
	for (int element = 0; element < 3; element++) {																			//idは敵の区別
		D3DXVECTOR3 enemy_vector(enemydata[element].GetX(), enemydata[element].GetY(), enemydata[element].GetZ());			//敵のベクトル
		VLenght = enemy_vector - player_vector;																				//二つのベクトルの差
		Lenght = D3DXVec3Length(&VLenght);

		/*データ送信(当たった場合)*/
		if (Lenght <= 2.0f) {
			char encode[BYTESIZE];									//暗号データ
			char senddata[BYTESIZE];								//送信データ

			/*データの生成*/
			AttckData data;
			data.base.size = sizeof(AttckData) - sizeof(int);
			data.base.id = 0x17;
			data.socket = element;
			
			/*暗号処理*/
			int encode_size = cipher->GetOpenSSLAES()->Encode(encode, (char*)&data, sizeof(AttckData));
			memcpy(senddata, &encode_size, sizeof(int));
			memcpy(&senddata[sizeof(int)], encode, encode_size);
			/*送信処理*/
			send(CLIENT.GetSocket(), (char*)&senddata, sizeof(int) + encode_size, 0);
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
	return data_qlist.front();
}

void Client::DeleteData()
{
	data_qlist.pop();
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
	if (data_qlist.empty() == true)return true;
	return false;
}

void Client::SetEnemyData(int _enemyid, Data * data)
{
	enemydata[_enemyid] = *data;
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


void Client::DataManipulate(const std::vector<char>* _data)
{
	Data data;
	char id = *(char*)&_data->at(0);					//なんのデータかidで判断する

	switch (id) {
	/*座標更新処理*/
	case 0x16: {
		float recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 0);
		data.SetX(recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 1);
		data.SetY(recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 2);
		data.SetZ(recvdata);
		recvdata = *(float*)&_data->at(sizeof(char) + sizeof(float) * 3);
		data.SetAngle(recvdata);
		recvdata = *(int*)&_data->at(sizeof(char) + sizeof(float) * 3 + sizeof(int));
		data.SetAnimation(recvdata);
		Lock();
		data_qlist.push(data);
		Unlock();
		break;
	}

	/*攻撃処理*/
	case 0x18:
		data.SetX(0.0f);					//敵の現在の座標
		data.SetY(0.0f);
		data.SetZ(0.0f);
		data.SetAngle(0);
		data.SetAnimation(DAMAGE);
		Lock();
		data_qlist.push(data);
		Unlock();
		break;
	}

}

