#ifndef Socket_h
#define Socket_h

class Socket{
public:
	Socket();
	~Socket();

	//---------------------------------------------------------
	//インスタンスを生成するタイミングで通信方法などを設定する
	//*～Createを最後に呼ぶ*
	//---------------------------------------------------------
	Socket & SetProtocolVersion_IPv4();										//IPv4
	Socket & SetProtocolVersion_IPv6();										//IPv6
	Socket & SetProtocolVersion_Dual();										//IPv4とIPv6の両方と通信可能(クライアントでのみ使用可能)
	Socket & SetProtocol_TCP();												//TCP
	Socket & SetProtocol_UDP();												//UDP
	Socket & SetIpAddress(std::string _addrs);								//IPアドレスの設定
	Socket & SetPortNumber(std::string _port);								//ポート番号の設定
	Socket & SetAsynchronous();												//非同期設定
	std::shared_ptr<Socket> ServerCreate(std::shared_ptr<Socket> _this);	//サーバーの生成
	std::shared_ptr<Socket> ClientCreate(std::shared_ptr<Socket> _this);	//クライアントの生成

	//---------------------------------------------------------
	//通信後各処理を行う
	//---------------------------------------------------------
	SOCKET Accept();														//接続に来たソケットを取得(サーバーで使用)
	int Recv(char* _recvbuf, int recvbuf_size, int flg = 0);				//データ受信(サーバーで使用)
	void Close();															//通信を切断する
	SOCKET GetSocket();														//ソケットを返す


private:
	//---------------------------------------------------------
	//共通で使用
	//---------------------------------------------------------
	bool AddressSet();
	bool Create();

	//---------------------------------------------------------
	//サーバーで使用
	//---------------------------------------------------------
	bool Bind();
	bool Listen();

	//---------------------------------------------------------
	//クライアントで使用
	//---------------------------------------------------------
	bool Connect();

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::unique_ptr<std::string> ip;										//IPアドレス
	std::unique_ptr<std::string> port;										//ポート番号
	SOCKET socket;															//ソケット
	bool asynchronousFlg = false;											//非同期フラグ
	struct addrinfo *result = NULL, hints;									//ソケット設定用変数

};

#endif
