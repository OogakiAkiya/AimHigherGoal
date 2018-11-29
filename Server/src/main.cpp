#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv)
{
	CurlWrapper* curl;
	curl = new CurlWrapper();
	curl->DBGetPos(NULL,"test");
	delete curl;

	Server* server= new Server();								//インスタンスの生成
	server->AcceptLoop();										//サーバー処理
	delete server;												//解放処理
}

