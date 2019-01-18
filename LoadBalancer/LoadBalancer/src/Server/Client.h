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
	NamedPipe::PipeData* GetOutputPipeData();								//OutputPipe送信用の先頭データ取得
	void DeletePipeData();													//OutputPipe送信用の先頭データ削除
	bool EmptyPipeData();													//OutputPipe送信用に値が入っているかどうか調べる(空なら:true,値が入っていれば:false)
private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void RecvLoop();														//受信処理

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 516;										//受信に使用する配列データのサイズ

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	int state=0;															//ソケットの状態を表す(0:通常,-1:デリート待ち)
	std::vector<char> tempDataList;											//一時データ配列
	std::shared_ptr<Data> data;												//ユーザーの各データが格納されている
	std::shared_ptr<std::queue<NamedPipe::PipeData>> outputPipeDataList;	//outputPipeで送信するデータリスト
};

#endif