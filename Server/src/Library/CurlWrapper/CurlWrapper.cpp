#include"../../Include.h"
#include"../Data/Data.h"
#include"../Mutex/ExtensionMutex.h"
#include "CurlWrapper.h"
#pragma comment(lib,"libcurl.dll.a")

size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data);

CurlWrapper::CurlWrapper()
{
	curl = curl_easy_init();
}

CurlWrapper::~CurlWrapper()
{
	//解放処理
	if (thread != nullptr)thread->detach();
	thread = nullptr;
	curl_easy_cleanup(curl);
	curl = nullptr;
}

void CurlWrapper::HTTPConnect(std::string* _data, std::string _url, std::string _postData)
{
	CURL* tempCurl;																			//URLへのアクセスに必要な設定などが入る
	tempCurl = curl_easy_init();

	//ユーザー追加処理
	if (tempCurl == NULL)return;
	std::string buf;																		//受け取ったデータを格納する
	std::string error;


	//接続設定
	curl_easy_setopt(tempCurl, CURLOPT_URL,_url.c_str());
	curl_easy_setopt(tempCurl, CURLOPT_POST, 1);											//POST設定
	curl_easy_setopt(tempCurl, CURLOPT_POSTFIELDS,_postData.c_str());						//送信データの設定
	curl_easy_setopt(tempCurl, CURLOPT_WRITEFUNCTION, BufferWriter);						//書込み関数設定
	curl_easy_setopt(tempCurl, CURLOPT_WRITEDATA, &buf);									//書込み変数設定

																							//送信
	code = curl_easy_perform(tempCurl);														//URLへの接続

	//送信失敗したかの判断
	if (code != CURLE_OK) {
		printf("curl ErrorCode=%d\n", code);
		return;
	}

	if (_data == nullptr) {
		curl_easy_cleanup(tempCurl);
		return;
	}
	*_data = buf;
	curl_easy_cleanup(tempCurl);

}




size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data)
{
	_data->append((char*)_ptr, _size * _nmemb);
	return _size * _nmemb;
}