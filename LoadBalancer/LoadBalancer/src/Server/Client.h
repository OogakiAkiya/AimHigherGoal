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
	std::shared_ptr<Data> GetData();
	int GetState();
	void SetPosGetFlg();
	bool GetPosGetFlg();
	//---------------------------------------------------------
	//完全データを扱う関数
	//---------------------------------------------------------
	NamedPipe::PipeData* GetPipeData();					//完全データの先頭データ取得
	void DeletePipeData();								//完全データの先頭データ削除
	bool EmptyPipeData();								//完全データに値が入っているかどうか調べる(空なら:true,値が入っていれば:false)

private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void RecvLoop();

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 516;
	static const int EXCHANGEKEY = 1;
	static const int CREATECOMPLETEDATA = 2;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	int state=0;															//ソケットの状態を表す(0:通常,-1:デリート待ち)
	bool keyChangeFlg = false;
	bool posGetFlg = false;
	std::vector<char> tempDataList;											//一時データ配列
	std::shared_ptr<Data> data;												//ユーザーの各データが格納されている
	std::unique_ptr<std::queue<NamedPipe::PipeData>> pipeDataList;
};

#endif