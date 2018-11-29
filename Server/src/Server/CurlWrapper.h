#ifndef CurlWrapper_h
#define CurlWrapper_h

class CurlWrapper
{
public:
	CurlWrapper();
	~CurlWrapper();
	void HttpConnect(Data* _data);											//Http通信を行う
	void CurlWrapper::StartThread(CurlWrapper* _curl, Data* _data);			//スレッド開始処理
	void DBGetPos(char* _data, std::string _userId);
	//---------------------------------------------------------
	//ランチャー
	//---------------------------------------------------------
	static void HttpLauncher(void* _args,Data* _data) {
		reinterpret_cast<CurlWrapper*>(_args)->HttpConnect(_data);			// 無理やりvoid*型にキャストして、本命の処理を実行する。
	}

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	CURL* curl;																//URLへのアクセスに必要な設定などが入る
	CURLcode code;															//処理に成功したかエラーチェックに使う
	std::thread thread;												//別スレッド処理
};

struct RecvBuffer {
	std::vector<char>* data;
	int dataSize;
};

#endif