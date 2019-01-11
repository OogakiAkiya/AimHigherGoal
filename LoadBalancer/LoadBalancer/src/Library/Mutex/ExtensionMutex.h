#ifndef ExtensionMutex_h
#define ExtensionMutex_h

class ExtensionMutex
{
public:
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
	//���[�J���֐�
	//---------------------------------------------------------
	ExtensionMutex();
	~ExtensionMutex();

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<std::mutex> mtx;

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static ExtensionMutex* s_Instance;
	ExtensionMutex & operator=(ExtensionMutex &);
	ExtensionMutex(ExtensionMutex&);

};
#define MUTEX ExtensionMutex::GetInstance()

#endif