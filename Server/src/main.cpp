#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv)
{
	Server* server= new Server();								//インスタンスの生成
	if (server != nullptr) {
		server->AcceptLoop();										//サーバー処理
		delete server;												//解放処理
	}
}

