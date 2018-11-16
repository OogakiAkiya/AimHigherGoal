#pragma once
class ExtensionMutex
{
public:
	ExtensionMutex();
	~ExtensionMutex();
	void Lock();								//�r������J�n
	void Unlock();								//�r������I��
	bool TryLock();								//�r�����䂪�ł������ǂ�����Ԃ�							

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static ExtensionMutex & GetInstance();
	static void DeleteInstance();

private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::mutex mtx;

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static ExtensionMutex* s_Instance;
	ExtensionMutex & operator=(ExtensionMutex &);
	ExtensionMutex(ExtensionMutex&);

};
#define MUTEX ExtensionMutex::GetInstance()
