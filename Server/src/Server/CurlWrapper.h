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
	void HTTPConnect(std::string* _data, std::string _url,std::string _postData);		//HTTP通信を行う
	void PosUpdataLoop(std::shared_ptr<Data> _data);									//座標の更新処理
	void StartThread(std::shared_ptr<CurlWrapper> _curl, std::shared_ptr<Data> _data);	//スレッド開始処理
	void CurlWrapper::DBGetPos(char* _data, std::shared_ptr<std::string> _userId);		//DBから値を受け取り第一引数に格納する

	//---------------------------------------------------------
	//ランチャー
	//---------------------------------------------------------
	static void HttpLauncher(void* _args,std::shared_ptr<Data> _data) {
		reinterpret_cast<CurlWrapper*>(_args)->PosUpdataLoop(_data);			// 無理やりvoid*型にキャストして、本命の処理を実行する。
	}

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	CURL* curl;																//URLへのアクセスに必要な設定などが入る
	CURLcode code;															//処理に成功したかエラーチェックに使う
	bool endFlg=true;
	std::shared_ptr <std::thread> thread;														//別スレッド処理
	std::shared_ptr<std::string> userId;
};

struct RecvBuffer {
	std::vector<char>* data;
	int dataSize;
};

#endif