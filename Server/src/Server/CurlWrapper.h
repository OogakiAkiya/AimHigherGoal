#ifndef CurlWrapper_h
#define CurlWrapper_h

class CurlWrapper
{
public:
	CurlWrapper();
	~CurlWrapper();

	//---------------------------------------------------------
	//データの送受信
	//---------------------------------------------------------
	void HTTPConnect(std::string* _data, std::string _url,std::string _postData);	//HTTP通信を行う
	void PosUpdataLoop(Data* _data);												//座標の更新処理
	void CurlWrapper::StartThread(CurlWrapper* _curl, Data* _data);					//スレッド開始処理
	void DBGetPos(char* _data, std::string _userId);								//DBから値を受け取り第一引数に格納する

	//---------------------------------------------------------
	//ランチャー
	//---------------------------------------------------------
	static void HttpLauncher(void* _args,Data* _data) {
		reinterpret_cast<CurlWrapper*>(_args)->PosUpdataLoop(_data);			// 無理やりvoid*型にキャストして、本命の処理を実行する。
	}

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	CURL* curl;																//URLへのアクセスに必要な設定などが入る
	CURLcode code;															//処理に成功したかエラーチェックに使う
	bool endFlg=true;
	std::thread thread;														//別スレッド処理
};

struct RecvBuffer {
	std::vector<char>* data;
	int dataSize;
};

#endif