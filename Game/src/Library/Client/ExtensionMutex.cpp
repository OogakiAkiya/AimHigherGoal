#include"../StandardLibraryInclude.h"
#include "ExtensionMutex.h"



ExtensionMutex::ExtensionMutex()
{
	mtx = std::make_unique<std::mutex>();
}


ExtensionMutex::~ExtensionMutex()
{
	mtx = nullptr;
}

void ExtensionMutex::Lock()
{
	mtx->lock();
}

void ExtensionMutex::Unlock()
{
	mtx->unlock();
}

bool ExtensionMutex::TryUnlock()
{
	if (mtx->try_lock() == false) {
		//¸”s
		return false;
	}
	//¬Œ÷
	return true;
}

