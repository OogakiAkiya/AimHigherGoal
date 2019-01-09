#ifndef Client_h
#define Client_h

class Client
{
public:
	Client();
	~Client();
	void Update();

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	SOCKET GetSocket();
	int GetRoomNumber();
	std::shared_ptr<Data> GetData();
	int GetState();
	OpenSSLAES* GetAES();
	CurlWrapper* GetCurl();
	void SetSocket(SOCKET _socket);
	void SetNumber(int _number);
	void SetPosGetFlg();
	bool GetPosGetFlg();
	//---------------------------------------------------------
	//完全データを扱う関数
	//---------------------------------------------------------
	std::vector<char>* GetCompleteData();					//完全データの先頭データ取得
	void DeleteCompleteData();								//完全データの先頭データ削除
	bool EmptyCompleteData();								//完全データに値が入っているかどうか調べる(空なら:true,値が入っていれば:false)

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
	static const int BYTESIZE = 516;
	static const int EXCHANGEKEY = 1;
	static const int CREATECOMPLETEDATA = 2;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	int roomNumber=0;
	int state=0;															//ソケットの状態を表す(0:通常,-1:デリート待ち)
	bool keyChangeFlg = false;
	bool posGetFlg = false;
	std::unique_ptr<std::queue<std::vector<char>>> completeDataQueList;		//完全データ配列
	std::vector<char> tempDataList;											//一時データ配列
	std::shared_ptr<Data> data;												//ユーザーの各データが格納されている
	std::unique_ptr<OpenSSLAES> aes;										//共通鍵暗号クラス
	std::unique_ptr<CurlWrapper> curl=nullptr;								//Http通信クラス
	std::unique_ptr<std::thread> thread = nullptr;
};

#endif