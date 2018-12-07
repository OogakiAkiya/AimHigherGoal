#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv)
{
	std::unique_ptr<Server> server=std::make_unique<Server>();
	if (server != nullptr) {
		server->AcceptLoop();										//サーバー処理

	}

}

