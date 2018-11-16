#include"../StandardLibraryInclude.h"
#include "ExtensionMutex.h"



ExtensionMutex::ExtensionMutex()
{
}


ExtensionMutex::~ExtensionMutex()
{
}

void ExtensionMutex::Lock()
{
	//std::lock_guard<std::mutex> lock(mtx);
	mtx.lock();
}

void ExtensionMutex::Unlock()
{
	mtx.unlock();
}

bool ExtensionMutex::TryUnlock()
{
	if (mtx.try_lock() == false) {
		//���s
		return false;
	}
	return true;
}

