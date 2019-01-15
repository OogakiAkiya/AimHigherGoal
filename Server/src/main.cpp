#include"Include.h"
#include"Library/Mutex/ExtensionMutex.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv[])
{
	ExtensionMutex::GetInstance();
	std::unique_ptr<Server> server;
	server = std::make_unique<Server>();

	if (server != nullptr) {
		while (1) {
			server->Update();										//サーバー処理
		}
	}
	ExtensionMutex::DeleteInstance();
}

