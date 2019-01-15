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
	~NamedPipe();
	bool CreateServer(std::string _pipeName);
	bool CreateClient(std::string _pipeName);
	bool ConnectRecv();
	int Read(char* _data, int _dataLength);
	int Write(char* _data, int _dataLengeh);

	void CreateInputPipe(std::string _pipeName,std::queue<PipeData>* _dataList);

private:
	HANDLE pipeHandle = INVALID_HANDLE_VALUE;
	bool deleteFlg=false;									//デストラクタでserverかclientかを判断する
};