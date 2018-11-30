#include"../Include.h"
#include"../Library/Data.h"
#include"../Library/ExtensionMutex.h"
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
	MUTEX.Lock();
	curl_easy_cleanup(curl);
	MUTEX.Unlock();
	thread.join();
}


void CurlWrapper::HttpConnect(Data* _data)
{
	//���[�U�[�ǉ�����
	if (curl == NULL)return;
	std::stringstream query;
	std::string output;

	//�ڑ��ݒ�
	curl_easy_setopt(curl, CURLOPT_URL, "http://lifestyle-qa.com/update_user_data.php");
	curl_easy_setopt(curl, CURLOPT_POST, 1);


	while(curl){
		//���b�Z�[�W�̐���
		query << "player=" << _data->GetSocket();
		query << "&x=" << _data->GetX();
		query << "&y=" << _data->GetY();
		query << "&z=" << _data->GetZ();
		query >> output;

		//���M
		if (curl!=NULL) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.c_str());
			code = curl_easy_perform(curl);								//URL�ւ̐ڑ�
		}
		else {
			break;
		}

		//���M���s�������̔��f
		if (code != CURLE_OK) {
			printf("code=%d\n", code);
			return;
		}

		//����������N���[��
		query.str("");
		query.clear(std::stringstream::goodbit);
		output.clear();

		Sleep(1000);													//1�b�҂�
	}

}

void CurlWrapper::StartThread(CurlWrapper* _curl,Data* _data)
{
	thread = std::thread(HttpLauncher,(void*)_curl,_data);
}

void CurlWrapper::DBGetPos(char* _data,std::string _userId)
{
	CURL* tempCurl;																//URL�ւ̃A�N�Z�X�ɕK�v�Ȑݒ�Ȃǂ�����
	tempCurl = curl_easy_init();

	//���[�U�[�ǉ�����
	if (tempCurl == NULL)return;
	std::stringstream query;
	std::string output = "";																//���M�p�f�[�^
	std::string buf;																		//�󂯎�����f�[�^���i�[����
	std::string error;
	char recvdata[256];

	//���M�f�[�^�̐���
	query << "player=" << _userId.c_str();
	query >> output;

	//�ڑ��ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_URL, "http://lifestyle-qa.com/get_pos.php");
	curl_easy_setopt(tempCurl, CURLOPT_POST, 1);											//POST�ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_POSTFIELDS, output.c_str());							//���M�f�[�^�̐ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_WRITEFUNCTION, TestBufferWriter);					//�����݊֐��ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_WRITEDATA, &buf);									//�����ݕϐ��ݒ�

	//���M
	code = curl_easy_perform(tempCurl);														//URL�ւ̐ڑ�

	//���M���s�������̔��f
	if (code != CURLE_OK) {
		printf("code=%d\n", code);
		return;
	}

	//json������
	auto json = json11::Json::parse(buf,error);
	float x = std::stof(json["x"].string_value());
	float y = std::stof(json["y"].string_value());
	float z = std::stof(json["z"].string_value());

	//�f�[�^���
	memcpy(recvdata, &x, sizeof(float));
	memcpy(&recvdata[sizeof(float)], &y, sizeof(float));
	memcpy(&recvdata[sizeof(float)*2], &z, sizeof(float));
	memcpy(_data, recvdata, sizeof(float) * 3);

	curl_easy_cleanup(tempCurl);

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

