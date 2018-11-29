#include"../Include.h"
#include"../Library/Data.h"
#include "CurlWrapper.h"
#pragma comment(lib,"libcurl.dll.a")

size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, void * _stream);
size_t TestBufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data);

CurlWrapper::CurlWrapper()
{
	curl = curl_easy_init();
}

CurlWrapper::~CurlWrapper()
{
	curl_easy_cleanup(curl);
	thread.detach();
}


void CurlWrapper::HttpConnect(Data* _data)
{
	//ユーザー追加処理
	if (curl == NULL)return;
	std::stringstream query;
	std::string output;

	//接続設定
	curl_easy_setopt(curl, CURLOPT_URL, "http://lifestyle-qa.com/update_user_data.php");
	curl_easy_setopt(curl, CURLOPT_POST, 1);


	while(curl){
		//メッセージの生成
		query << "player=" << _data->GetSocket();
		query << "&x=" << _data->GetX();
		query << "&y=" << _data->GetY();
		query << "&z=" << _data->GetZ();
		query >> output;
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.c_str());

		//送信
		if (curl!=NULL) {
			code = curl_easy_perform(curl);								//URLへの接続
		}
		else {
			break;
		}

		//送信失敗したかの判断
		if (code != CURLE_OK) {
			printf("code=%d\n", code);
			return;
		}

		//文字列内をクリーン
		query.str("");
		query.clear(std::stringstream::goodbit);
		output.clear();

		Sleep(1000);													//1秒待つ
	}

}

void CurlWrapper::StartThread(CurlWrapper* _curl,Data* _data)
{
	thread = std::thread(HttpLauncher,(void*)_curl,_data);
}

void CurlWrapper::DBGetPos(char* _data,std::string _userId)
{
	//ユーザー追加処理
	if (curl == NULL)return;
	std::stringstream query;
	std::string output = "";																//送信用データ
	std::string buf;																		//受け取ったデータを格納する
	std::string error;

	//送信データの生成
	query << "player=" << _userId.c_str();
	query >> output;

	//接続設定
	curl_easy_setopt(curl, CURLOPT_URL, "http://lifestyle-qa.com/get_pos.php");
	curl_easy_setopt(curl, CURLOPT_POST, 1);											//POST設定
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.c_str());							//送信データの設定
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, TestBufferWriter);					//書込み関数設定
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);									//書込み変数設定

	//送信
	code = curl_easy_perform(curl);														//URLへの接続

	//送信失敗したかの判断
	if (code != CURLE_OK) {
		printf("code=%d\n", code);
		return;
	}

	//jsonを扱う
	auto json = json11::Json::parse(buf,error);
	float test = std::stof(json["x"].string_value());
	float test2 = std::stof(json["y"].string_value());
	printf("%f,%f", test,test2);
	

}

size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, void * _stream)
{
	/*
	RecvBuffer *buf=(RecvBuffer*)_stream;
	int block = _size*_nmemb;
	if (!buf)return block;

	buf->data->resize(block);
	buf->data->push_back(*_ptr);
	buf->dataSize = block;
	*/
	int block = _size*_nmemb;
	memcpy(_stream, _ptr, block);

	return block;
}

size_t TestBufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data)
{
	_data->append((char*)_ptr, _size * _nmemb);
	return _size * _nmemb;
}

