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

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	CURL* curl;																//URLへのアクセスに必要な設定などが入る
	CURLcode code;															//処理に成功したかエラーチェックに使う
	bool endFlg=true;
	std::unique_ptr <std::thread> thread;									//別スレッド処理
	std::shared_ptr<std::string> userId;
};

struct RecvBuffer {
	std::vector<char>* data;
	int dataSize;
};

#endif