#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update(std::queue<NamedPipe::PipeData>* _dataList);				//更新処理
	void SendAllClient(char* _data,int _dataLength);

private:
	void AcceptSocket();													//accept処理
	void GetOutputPipeData(std::queue<NamedPipe::PipeData>* _dataList);		//パイプに送る用のデータを取得
	void SocketUpdate();													//各ソケットの更新処理
	void ControllerUpdate();												//スレッドが動作させる関数

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 1280;
	const char* PORTNUMBER = "49155";										//ポート番号

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::shared_ptr<Socket> socket = nullptr;								//Accept用ソケット
	std::vector<std::shared_ptr<Client>> socketList;						//ソケット配列
	std::unique_ptr<std::thread> thread=nullptr;
	std::vector<Client*> sendList;

	bool threadFlg = true;
};

#endif