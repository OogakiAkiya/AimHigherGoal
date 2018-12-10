#ifndef ExtensionMutex_h
#define ExtensionMutex_h

class ExtensionMutex
{
public:
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
	//ローカル関数
	//---------------------------------------------------------
	ExtensionMutex();
	~ExtensionMutex();

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::unique_ptr<std::mutex> mtx;

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static ExtensionMutex* s_Instance;
	ExtensionMutex & operator=(ExtensionMutex &);
	ExtensionMutex(ExtensionMutex&);

};
#define MUTEX ExtensionMutex::GetInstance()

#endif