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
	curl_easy_cleanup(curl);
	curl = nullptr;
}

void CurlWrapper::HTTPConnect(std::string* _data, std::string _url, std::string _postData)
{
	//ユーザー追加処理
	if (curl == NULL)return;
	std::string buf;																		//受け取ったデータを格納する

	//接続設定
	curl_easy_setopt(curl, CURLOPT_URL,_url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);											//POST設定
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,_postData.c_str());						//送信データの設定
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BufferWriter);						//書込み関数設定
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);									//書込み変数設定

																							//送信
	code = curl_easy_perform(curl);														//URLへの接続

	//送信失敗したかの判断
	if (code != CURLE_OK) {
		printf("curl ErrorCode=%d\n", code);
		return;
	}

	if (_data != nullptr)*_data = buf;
}




size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data)
{
	_data->append((char*)_ptr, _size * _nmemb);
	return _size * _nmemb;
}