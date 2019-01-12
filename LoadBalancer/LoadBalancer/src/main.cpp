#include "Include.h"
#include"Library/Data/Data.h"
#include"Library/Mutex/ExtensionMutex.h"
#include"Library/Process/Process.h"
#include"Library/NamedPipe/NamedPipe.h"
#include"Library/Socket/Socket.h"
#include"Server/Client.h"
#include"Server/ClientController.h"

#include"Library/LoadBalancer/LoadBalancer.h"

void main()
{
	
	ExtensionMutex::GetInstance();
	std::unique_ptr<LoadBalancer> loadBalancer=std::make_unique<LoadBalancer>();
	while (1) {
		loadBalancer->Updata();
	}
	loadBalancer = nullptr;
	ExtensionMutex::DeleteInstance();
}