#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv)
{
	/*
	CurlWrapper* curl;
	curl = new CurlWrapper();
	char data[255];
	curl->DBGetPos(data,"test");
	float* a = (float*)data;
	printf("%f",*a);
	delete curl;
	*/

	Server* server= new Server();								//�C���X�^���X�̐���
	server->AcceptLoop();										//�T�[�o�[����
	delete server;												//�������
}

