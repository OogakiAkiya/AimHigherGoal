#pragma once
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

	std::mutex mtx;

};
