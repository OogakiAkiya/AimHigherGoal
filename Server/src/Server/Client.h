#ifndef Client_h
#define Client_h

class Client
{
public:
	Client();
	~Client();
	void Recv();											//受信処理
	void StartRecvThread(Client* _client);					//Recvスレッドの開始
	void StartHttpThread();									//Http通信スレッド開始

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	SOCKET GetSocket();
	int GetRoomNumber();
	Data* GetData();
	int GetState();
	OpenSSLAES* GetAES();
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
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void RecvLoop(int _loopType);
	bool ExchangeKey();
	void CreateCompleteData();

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 256;
	static const int EXCHANGEKEY = 1;
	static const int CREATECOMPLETEDATA = 2;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	int roomNumber=0;
	int state=0;											//ソケットの状態を表す(0:通常,-1:デリート待ち)
	std::thread* thread;
	std::queue<std::vector<char>> completeDataQueList;		//完全データ配列
	std::vector<char> tempDataList;							//一時データ配列
	Data* data;												//ユーザーの各データが格納されている
	OpenSSLAES* aes;										//共通鍵暗号クラス
	CurlWrapper* curl;										//Http通信クラス
};

#endif