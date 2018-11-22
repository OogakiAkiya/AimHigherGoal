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
	//����
	return true;
}

