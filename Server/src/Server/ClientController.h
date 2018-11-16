#pragma once
class Client;

class ClientController
{
public:
	ClientController();
	~ClientController();
	void SetSocket(Client* _socket);										//�\�P�b�g���X�g�ɒǉ�
	bool SerchNumber(int _number);											//�����Ɠ������[���i���o�[�����݂��邩���ׂ�
	void StartThread(ClientController* _socket_controller);					//�X���b�h�J�n
	void ControllerThread();												//�X���b�h�����삳����֐�

	//---------------------------------------------------------
	//�����`���[
	//---------------------------------------------------------
	static void ControllerThreadLauncher(void* args) {
		reinterpret_cast<ClientController*>(args)->ControllerThread();		// �������void*�^�ɃL���X�g���āA�{���̏��������s����B
	}

private:
	//---------------------------------------------------------
	//���̃N���X���ł����g�p���Ȃ��֐�
	//---------------------------------------------------------
	void DataManipulate(Client* _socket,std::vector<char>* _data);

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::vector<Client*> socketlist;										//�\�P�b�g�z��
	std::vector<int> room_numberlist;										//�������ꂽ���[���ԍ���ۑ�
	std::thread* thread;													//�X���b�h
};

////////////////////////////////////////////////////////////////////
//�ȉ����M�f�[�^�\��
////////////////////////////////////////////////////////////////////
#pragma pack(1)
//�f�[�^���M�̍Œ��
struct BaseData
{
	int size;
	char id;
};

//���W�f�[�^
struct PosData {
	int size;
	char id;
	float x;
	float y;
	float z;
	float angle;
	int animation;
};
#pragma pack()

