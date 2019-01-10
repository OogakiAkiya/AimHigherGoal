#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv[])
{
	if (argc > 1) {
		int iNum = atoi(argv[1]);
		printf("ポート番号=%d\n",iNum);
	}
	std::unique_ptr<Server> server=std::make_unique<Server>();
	if (server != nullptr) {
		server->AcceptLoop();										//サーバー処理
		server = nullptr;
	}

}

