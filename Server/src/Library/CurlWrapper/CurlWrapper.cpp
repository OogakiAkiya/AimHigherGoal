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

void CurlWrapper::HTTPConnect(std::string* _data, std::string _url, std::string _postData,std::string _userID)
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
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);										//�^�C���A�E�g
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3L);									//�ڑ��^�C���A�E�g

																							//���M
	code = curl_easy_perform(curl);														//URL�ւ̐ڑ�

	//���M���s�������̔��f
	if (code != CURLE_OK) {
		switch (code) {
		case CURLE_BAD_FUNCTION_ARGUMENT:
		case CURLE_OPERATION_TIMEDOUT:
			printf("%s���^�C���A�E�g���܂���\n", _userID.c_str());
			break;
		case CURLE_RECV_ERROR:
			printf("%s���l�b�g���[�N�f�[�^�̎�M�Ɏ��s���܂���", _userID.c_str());
			break;
		default:
			printf("%s:curl ErrorCode=%d\n", _userID.c_str(),code);
			break;
		}

	}

	if (_data != nullptr)*_data = buf;
}




size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data)
{
	_data->append((char*)_ptr, _size * _nmemb);
	return _size * _nmemb;
}