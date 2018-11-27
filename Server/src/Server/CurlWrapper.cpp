#include"../Include.h"
#include"../Library/Data.h"
#include "CurlWrapper.h"

#pragma comment(lib,"libcurl.dll.a")

CurlWrapper::CurlWrapper()
{
	curl = curl_easy_init();
}

CurlWrapper::~CurlWrapper()
{
	curl_easy_cleanup(curl);
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
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.c_str());

		//���M
		if (curl!=NULL) {
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
