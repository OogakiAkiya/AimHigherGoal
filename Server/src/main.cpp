#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv)
{
	CurlWrapper* curl;
	curl = new CurlWrapper();
	curl->DBGetPos(NULL,"test");
	delete curl;

	Server* server= new Server();								//�C���X�^���X�̐���
	server->AcceptLoop();										//�T�[�o�[����
	delete server;												//�������
}

