#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update(std::queue<NamedPipe::PipeData>* _dataList);
	void ControllerUpdate();												//スレッドが動作させる関数
	void SocketUpdate();
	void GetPipeData(std::queue<NamedPipe::PipeData>* _dataList);
private:
	void AcceptSocket();


	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 128;
	const char* PORTNUMBER = "49155";						//ポート番号


	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;
	std::vector<std::shared_ptr<Client>> socketList;										//ソケット配列
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