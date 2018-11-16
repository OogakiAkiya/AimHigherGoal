#pragma once
class ExtensionMutex
{
public:
	ExtensionMutex();
	~ExtensionMutex();
	void Lock();
	void Unlock();
	bool TryUnlock();

private:
	std::mutex mtx;

};
