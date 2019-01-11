#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv[])
{
	std::unique_ptr<Server> server;

	if (argc > 1) {
		int port = atoi(argv[1]);
		printf("ポート番号=%d\n",port);
		std::stringstream query;						//webサーバーに送るデータ
		query << port;
		server = std::make_unique<Server>(query.str());
	}
	else {
		printf("デフォルトポート番号=49155\n");
		server = std::make_unique<Server>();
	}

	if (server != nullptr) {
		server->AcceptLoop();										//サーバー処理
		server = nullptr;
	}

}

