#include"Include.h"
#include"Library/Mutex/ExtensionMutex.h"
#include"LibraryInclude.h"

void main(int argc, char* argv[])
{
	int processNumber = atoi(argv[1]);

	ExtensionMutex::GetInstance();
	std::unique_ptr<Server> server;
	server = std::make_unique<Server>(processNumber);

	if (server != nullptr) {
		while (1) {
			server->Update();										//サーバー処理
		}
	}
	ExtensionMutex::DeleteInstance();
}

