class CurlWrapper
{
public:
	CurlWrapper();
	~CurlWrapper();
	void AddUser(SOCKET _id);
	void HttpConnect(Data* _data);
	
	void CurlWrapper::StartThread(CurlWrapper* _curl, Data* _data);

	static void HttpLauncher(void* _args,Data* _data) {
		reinterpret_cast<CurlWrapper*>(_args)->HttpConnect(_data);			// 無理やりvoid*型にキャストして、本命の処理を実行する。
	}
	
private:
	CURL* curl;
	CURLcode code;
	std::thread thread;
};

