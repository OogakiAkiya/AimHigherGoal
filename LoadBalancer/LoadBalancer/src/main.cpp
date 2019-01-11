#include "Include.h"
#include"Library/Mutex/ExtensionMutex.h"
#include"Library/Process/Process.h"
#include"Library/NamedPipe/NamedPipe.h"
#include"Library/LoadBalancer/LoadBalancer.h"

void main()
{
	//ExtensionMutex::GetInstance();
	std::unique_ptr<LoadBalancer> loadBalancer=std::make_unique<LoadBalancer>();
	loadBalancer->Updata();
	Sleep(20000);
	loadBalancer = nullptr;
	//ExtensionMutex::DeleteInstance();
}