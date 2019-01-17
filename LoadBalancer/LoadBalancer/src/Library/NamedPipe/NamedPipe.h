class NamedPipe {
public:
#pragma pack(1)

	struct PipeData
	{
		int byteSize;
		char data[260];						//256バイト+int型分
	};

#pragma pack()
	NamedPipe();
	NamedPipe(std::string _discriminationName);
	~NamedPipe();
	bool CreateServer(std::string _pipeName);
	bool CreateClient(std::string _pipeName);
	bool ConnectRecv();
	int Read(char* _data, int _dataLength);
	int Write(char* _data, int _dataLengeh);
	void CreateInputPipe(std::string _pipeName,std::queue<PipeData>* _dataList);
	void CountUp();
	void CountDown();
	int GetCount();
	std::string* GetPipeName();
private:
	int count = 0;
	HANDLE pipeHandle = INVALID_HANDLE_VALUE;
	bool deleteFlg=false;									//デストラクタでserverかclientかを判断する
	std::string pipeName;
	std::string discriminationName="";
};