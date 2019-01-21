#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update(std::queue<NamedPipe::PipeData>* _dataList);				//�X�V����
	void SendAllClient(char* _data,int _dataLength);

private:
	void AcceptSocket();													//accept����
	void GetOutputPipeData(std::queue<NamedPipe::PipeData>* _dataList);		//�p�C�v�ɑ���p�̃f�[�^���擾
	void SocketUpdate();													//�e�\�P�b�g�̍X�V����
	void ControllerUpdate();												//�X���b�h�����삳����֐�

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 1280;
	const char* PORTNUMBER = "49155";										//�|�[�g�ԍ�

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;								//Accept�p�\�P�b�g
	std::vector<std::shared_ptr<Client>> socketList;						//�\�P�b�g�z��
	std::unique_ptr<std::thread> thread=nullptr;
	std::vector<Client*> sendList;

	bool threadFlg = true;
};

#endif