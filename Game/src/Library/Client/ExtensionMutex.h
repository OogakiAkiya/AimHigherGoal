#ifndef ExtensionMutex_h
#define ExtensionMutex_h

class ExtensionMutex
{
public:
	ExtensionMutex();
	~ExtensionMutex();

	void Lock();				//�r������J�n
	void Unlock();				//�r������I��
	bool TryUnlock();			//�r����������݂ďo�����ꍇ�^��Ԃ�

private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<std::mutex> mtx;

};

#endif
