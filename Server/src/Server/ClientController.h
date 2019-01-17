#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update();
	void AddClient(std::shared_ptr<Client> _client);						//�\�P�b�g���X�g�ɒǉ�
	void SocketUpdate();
private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::vector<std::shared_ptr<Client>> clientList;										//�\�P�b�g�z��
	std::vector<int> roomNumberList;														//�������ꂽ���[���ԍ���ۑ�
};

#endif