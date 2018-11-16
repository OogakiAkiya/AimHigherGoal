#pragma once
class Client;

class ClientController
{
public:
	ClientController();
	~ClientController();
	void SetSocket(Client* _socket);										//ソケットリストに追加
	bool SerchNumber(int _number);											//引数と同じルームナンバーが存在するか調べる
	void StartThread(ClientController* _socket_controller);					//スレッド開始
	void ControllerThread();												//スレッドが動作させる関数

	//---------------------------------------------------------
	//ランチャー
	//---------------------------------------------------------
	static void ControllerThreadLauncher(void* args) {
		reinterpret_cast<ClientController*>(args)->ControllerThread();		// 無理やりvoid*型にキャストして、本命の処理を実行する。
	}

private:
	//---------------------------------------------------------
	//このクラス内でしか使用しない関数
	//---------------------------------------------------------
	void DataManipulate(Client* _socket,std::vector<char>* _data);

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::vector<Client*> socketlist;										//ソケット配列
	std::vector<int> room_numberlist;										//生成されたルーム番号を保存
	std::thread* thread;													//スレッド
};

////////////////////////////////////////////////////////////////////
//以下送信データ構成
////////////////////////////////////////////////////////////////////
#pragma pack(1)
//データ送信の最低限
struct BaseData
{
	int size;
	char id;
};

//座標データ
struct PosData {
	int size;
	char id;
	float x;
	float y;
	float z;
	float angle;
	int animation;
};
#pragma pack()

