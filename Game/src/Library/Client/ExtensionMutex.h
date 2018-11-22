#pragma once
class ExtensionMutex
{
public:
	ExtensionMutex();
	~ExtensionMutex();

	void Lock();				//排他制御開始
	void Unlock();				//排他制御終了
	bool TryUnlock();			//排他制御を試みて出来た場合真を返す

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------

	std::mutex mtx;

};
