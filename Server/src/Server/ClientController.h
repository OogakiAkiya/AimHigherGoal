#ifndef ClientController_h
#define ClientController_h

class ClientController
{
public:
	ClientController();
	~ClientController();
	void Update();
	void AddClient(std::shared_ptr<Client> _client);						//ソケットリストに追加
	void SocketUpdate();
private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 128;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::vector<std::shared_ptr<Client>> clientList;										//ソケット配列
	std::vector<int> roomNumberList;														//生成されたルーム番号を保存
};

#endif