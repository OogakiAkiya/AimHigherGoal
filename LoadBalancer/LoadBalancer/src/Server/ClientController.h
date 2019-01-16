#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update(std::queue<NamedPipe::PipeData>* _dataList);
	void ControllerUpdate();												//�X���b�h�����삳����֐�
	void SocketUpdate();
	void GetPipeData(std::queue<NamedPipe::PipeData>* _dataList);
	void SendAllClient(NamedPipe::PipeData* _data);
private:
	void AcceptSocket();


	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int BYTESIZE = 128;
	const char* PORTNUMBER = "49155";						//�|�[�g�ԍ�


	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;
	std::vector<std::shared_ptr<Client>> socketList;										//�\�P�b�g�z��
};

#endif