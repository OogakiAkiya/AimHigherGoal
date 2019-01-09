#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update();
	void SetSocket(std::shared_ptr<Client> _socket);						//ソケットリストに追加
	void ControllerThread();												//スレッドが動作させる関数
	void SocketThread();
private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void DataManipulate(Client* _socket,std::vector<char>* _data);
	void CreateDBData();
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