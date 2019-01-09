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
	//���[�U�[�ǉ�����
	if (curl == NULL)return;
	std::string buf;																		//�󂯎�����f�[�^���i�[����

	//�ڑ��ݒ�
	curl_easy_setopt(curl, CURLOPT_URL,_url.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);											//POST�ݒ�
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,_postData.c_str());						//���M�f�[�^�̐ݒ�
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, BufferWriter);						//�����݊֐��ݒ�
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);									//�����ݕϐ��ݒ�

																							//���M
	code = curl_easy_perform(curl);														//URL�ւ̐ڑ�

	//���M���s�������̔��f
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