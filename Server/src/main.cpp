#include"Include.h"
#include"LibraryInclude.h"
using namespace std;

void main(int argc, char* argv[])
{
	std::unique_ptr<Server> server;

	if (argc > 1) {
		int port = atoi(argv[1]);
		printf("�|�[�g�ԍ�=%d\n",port);
		std::stringstream query;						//web�T�[�o�[�ɑ���f�[�^
		query << port;
		server = std::make_unique<Server>(query.str());
	}
	else {
		printf("�f�t�H���g�|�[�g�ԍ�=49155\n");
		server = std::make_unique<Server>();
	}

	if (server != nullptr) {
		server->AcceptLoop();										//�T�[�o�[����
		server = nullptr;
	}

}

