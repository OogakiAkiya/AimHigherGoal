#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void SetSocket(std::shared_ptr<Client> _socket);										//ソケットリストに追加
	bool SerchNumber(int _number);											//引数と同じルームナンバーが存在するか調べる
	void StartThread(ClientController* _socketController);					//スレッド開始
	void ControllerThread();												//スレッドが動作させる関数

	//---------------------------------------------------------
	//ランチャー
	//---------------------------------------------------------
	static void ControllerThreadLauncher(void* args) {
		reinterpret_cast<ClientController*>(args)->ControllerThread();		// 無理やりvoid*型にキャストして、本命の処理を実行する。
	}

private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void DataManipulate(Client* _socket,std::vector<char>* _data);

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::vector<std::shared_ptr<Client>> socketList;										//ソケット配列
	std::vector<std::shared_ptr<Client>> addSocketPool;										//ソケット配列
	std::vector<int> roomNumberList;														//生成されたルーム番号を保存
	std::shared_ptr<std::thread> thread;													//スレッド
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

//ユーザー情報処理
struct UserData {
	BaseData data;
	float x;
	float y;
	float z;
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

#endif