class NamedPipe {
public:

#pragma pack(1)
	struct PipeData
	{
		int byteSize;
		char data[256];						//256バイト+int型分
	};
#pragma pack()

	NamedPipe();
	NamedPipe(std::string _discriminationName);
	~NamedPipe();
	bool CreateServer(std::string _pipeName);											//受信用パイプを作成
	bool CreateClient(std::string _pipeName);											//送信用パイプを作成
	bool ConnectRecv();																	//CreateServerの後に接続先を待つ
	int Read(char* _data, int _dataLength);												//_dataに受信したデータを受け取る
	int Write(char* _data, int _dataLengeh);											//_dataを送信する
	void CreateInputPipe(std::string _pipeName,std::queue<PipeData>* _dataList);		//受信用のスレッドを作成する
	void CountUp();																		//カウントを1増加させる
	void CountDown();																	//カウントを1減少させる
	int GetCount();																		//カウントを返す
	std::string* GetPipeName();															//パイプ名を返す
private:
	HANDLE pipeHandle = INVALID_HANDLE_VALUE;											//パイプハンドル
	std::string pipeName;																//パイプ名が入る
	std::string discriminationName = "";												//printfの時にパイプ名を識別するときに使用する
	int count = 0;																		//パイプに来た数
	bool deleteFlg = false;																//デストラクタでserverかclientかを判断する
};