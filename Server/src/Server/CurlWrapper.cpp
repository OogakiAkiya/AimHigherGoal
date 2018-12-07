#include"../Include.h"
#include"../Library/Data.h"
#include"../Library/ExtensionMutex.h"
#include "CurlWrapper.h"
#pragma comment(lib,"libcurl.dll.a")

size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data);

CurlWrapper::CurlWrapper()
{
	curl = curl_easy_init();
}

CurlWrapper::~CurlWrapper()
{
	//�������
	if(thread!=nullptr)thread->detach();
	thread = nullptr;
	curl_easy_cleanup(curl);
	curl = nullptr;
	userId = nullptr;
}

void CurlWrapper::HTTPConnect(std::string* _data, std::string _url, std::string _postData)
{
	CURL* tempCurl;																//URL�ւ̃A�N�Z�X�ɕK�v�Ȑݒ�Ȃǂ�����
	tempCurl = curl_easy_init();

	//���[�U�[�ǉ�����
	if (tempCurl == NULL)return;
	std::string buf;																		//�󂯎�����f�[�^���i�[����
	std::string error;


	//�ڑ��ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_URL,_url.c_str());
	curl_easy_setopt(tempCurl, CURLOPT_POST, 1);											//POST�ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_POSTFIELDS,_postData.c_str());							//���M�f�[�^�̐ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_WRITEFUNCTION, BufferWriter);					//�����݊֐��ݒ�
	curl_easy_setopt(tempCurl, CURLOPT_WRITEDATA, &buf);									//�����ݕϐ��ݒ�

																							//���M
	code = curl_easy_perform(tempCurl);														//URL�ւ̐ڑ�

	//���M���s�������̔��f
	if (code != CURLE_OK) {
		printf("code=%d\n", code);
		return;
	}

	*_data = buf;
	curl_easy_cleanup(tempCurl);

}


void CurlWrapper::PosUpdataLoop(std::shared_ptr<Data> _data)
{
	//���[�U�[�ǉ�����
	if (curl == NULL)return;
	std::stringstream query;
	std::string output;

	//�ڑ��ݒ�
	curl_easy_setopt(curl, CURLOPT_URL, "http://lifestyle-qa.com/update_user_data.php");
	curl_easy_setopt(curl, CURLOPT_POST, 1);


	while (curl) {
		//���b�Z�[�W�̐���
		if(userId==nullptr){return;}
		query << "player=" << userId->c_str();
		query << "&x=" << _data->GetX();
		query << "&y=" << _data->GetY();
		query << "&z=" << _data->GetZ();
		query >> output;

		//���M
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.c_str());			//���b�Z�[�W�ݒ�
		code = curl_easy_perform(curl);										//URL�ւ̐ڑ�

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

void CurlWrapper::StartThread(std::shared_ptr<CurlWrapper> _curl,std::shared_ptr<Data> _data)
{
	thread = std::make_shared<std::thread>(HttpLauncher, (void*)_curl.get(), _data);
}

void CurlWrapper::DBGetPos(char* _data, std::shared_ptr<std::string> _userId)
{
	//���[�U�[�ǉ�����
	std::stringstream query;
	std::string output = "";																//���M�p�f�[�^
	std::string buf;																		//�󂯎�����f�[�^���i�[����
	std::string error;
	char recvdata[256];

	//���M�f�[�^�̐���
	query << "player=" << _userId.get()->c_str();
	query >> output;

	userId = _userId;

	//�ڑ��ݒ�
	HTTPConnect(&buf, "http://lifestyle-qa.com/get_pos.php", output.c_str());

	//json����l�̎擾
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	if (buf.length() != 0) {
		auto json = json11::Json::parse(buf, error);

		x = std::stof(json["x"].string_value());
		y = std::stof(json["y"].string_value());
		z = std::stof(json["z"].string_value());
	}

	//�f�[�^���
	memcpy(recvdata, &x, sizeof(float));
	memcpy(&recvdata[sizeof(float)], &y, sizeof(float));
	memcpy(&recvdata[sizeof(float) * 2], &z, sizeof(float));
	memcpy(_data, recvdata, sizeof(float) * 3);

}

size_t BufferWriter(char * _ptr, size_t _size, size_t _nmemb, std::string* _data)
{
	_data->append((char*)_ptr, _size * _nmemb);
	return _size * _nmemb;
}