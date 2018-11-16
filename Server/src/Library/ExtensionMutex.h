#pragma once
class ExtensionMutex
{
public:
	ExtensionMutex();
	~ExtensionMutex();
	void Lock();								//排他制御開始
	void Unlock();								//排他制御終了
	bool TryLock();								//排他制御ができたかどうかを返す							

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static ExtensionMutex & GetInstance();
	static void DeleteInstance();

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::mutex mtx;

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static ExtensionMutex* s_Instance;
	ExtensionMutex & operator=(ExtensionMutex &);
	ExtensionMutex(ExtensionMutex&);

};
#define MUTEX ExtensionMutex::GetInstance()
