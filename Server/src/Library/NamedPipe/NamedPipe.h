class NamedPipe {
public:
	NamedPipe();
	~NamedPipe();
	bool CreateServer(std::string _pipeName);
	bool CreateClient(std::string _pipeName);
	bool ConnectRecv();
	int Read(char* _data, int _dataLength);
	int Write(char* _data, int _dataLengeh);
private:
	HANDLE pipeHandle = INVALID_HANDLE_VALUE;
	bool deleteFlg=false;									//デストラクタでserverかclientかを判断する
};