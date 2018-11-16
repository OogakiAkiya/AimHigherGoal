#include"../Include.h"
#include "ExtensionMutex.h"

ExtensionMutex* ExtensionMutex::s_Instance = nullptr;


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

bool ExtensionMutex::TryLock()
{
	if (mtx.try_lock() == false) {
		//Ž¸”s
		return false;
	}
	return true;
}

ExtensionMutex & ExtensionMutex::GetInstance()
{
	if (s_Instance == nullptr) { s_Instance = new ExtensionMutex(); }
	return *s_Instance;
}

void ExtensionMutex::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}
}

