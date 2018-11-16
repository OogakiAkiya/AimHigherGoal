#pragma once
class Client
{

public:
	Client();
	~Client();
	void Recv();											//受信処理
	void StartRecvThread(Client* _client);						//スレッドの開始
	void StartHttpThread();
	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	SOCKET GetSocket();
	int GetRoomNumber();
	Data* GetData();
	int GetState();
	void SetSocket(SOCKET _socket);
	void SetNumber(int _number);

	//---------------------------------------------------------
	//完全データを扱う関数
	//---------------------------------------------------------
	std::vector<char>* GetCompleteData();					//完全データの先頭データ取得
	void DeleteCompleteData();								//完全データの先頭データ削除
	bool EmptyCompleteData();								//完全データに値が入っているかどうか調べる(空なら:true,値が入っていれば:false)
	
	//---------------------------------------------------------
	//ランチャー
	//---------------------------------------------------------
	static void RecvLauncher(void* args) {
		reinterpret_cast<Client*>(args)->Recv();			// 無理やりvoid*型にキャストして、本命の処理を実行する。
	}

private:
	//void RoomMatch();

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	int room_number=0;
	int state=0;											//ソケットの状態を表す(0:通常,-1:デリート待ち)
	std::thread* thread;
	std::queue<std::vector<char>> completedata_qlist;		//完全データ配列
	std::vector<char> temp_datalist;						//一時データ配列
	Data* data;												//ユーザーの各データが格納されている

	CurlWrapper* curl;
};

