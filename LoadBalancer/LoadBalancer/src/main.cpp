#include "Include.h"
#include"LoadBalancer/LoadBalancer.h"
void main()
{
	std::unique_ptr<LoadBalancer> loadBalancer=std::make_unique<LoadBalancer>();
	//loadBalancer->CreateServer();
	loadBalancer->Updata();
	loadBalancer = nullptr;
}