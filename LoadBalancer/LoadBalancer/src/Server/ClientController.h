#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update(std::queue<NamedPipe::PipeData>* _dataList);
	void ControllerUpdate();												//スレッドが動作させる関数
	void SocketUpdate();
	void GetPipeData(std::queue<NamedPipe::PipeData>* _dataList);
	void SendAllClient(NamedPipe::PipeData* _data);
private:
	void AcceptSocket();


	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 128;
	const char* PORTNUMBER = "49155";						//ポート番号


	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;
	std::vector<std::shared_ptr<Client>> socketList;										//ソケット配列
};

#endif