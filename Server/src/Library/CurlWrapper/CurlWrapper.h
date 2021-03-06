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
	void HTTPConnect(std::string* _data, std::string _url, std::string _postData, std::string _userID);		//HTTP通信を行う

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	CURL* curl;																//URLへのアクセスに必要な設定などが入る
	CURLcode code;															//処理に成功したかエラーチェックに使う
};

#endif