class Socket {
public:
	Socket();
	~Socket();

	//---------------------------------------------------------
	//インスタンスを生成するタイミングで通信方法などを設定する
	//*～Createを最後に呼ぶ*
	//---------------------------------------------------------
	Socket & SetProtocolVersion_IPv4();							//IPv4
	Socket & SetProtocolVersion_IPv6();							//IPv6
	Socket & SetProtocolVersion_Dual();							//IPv4とIPv6の両方と通信可能(クライアントでのみ使用可能)
	Socket & SetProtocol_TCP();									//TCP
	Socket & SetProtocol_UDP();									//UDP
	Socket & SetIpAddress(std::string addrs);					//IPアドレスの設定
	Socket & SetPortNumber(std::string _port);					//ポート番号の設定
	Socket* ServerCreate();										//サーバーの生成
	Socket* ClientCreate(bool _asynchronousflg=false);										//クライアントの生成

	//---------------------------------------------------------
	//通信後各処理を行う
	//---------------------------------------------------------
	SOCKET Accept();											//接続に来たソケットを取得(サーバーで使用)
	int Recv(char* _recvbuf, int recvbuf_size, int flg = 0);	//データ受信(サーバーで使用)
	void Close();												//通信を切断する
	SOCKET GetSocket();											//ソケットを返す
	static Socket* Instantiate();

private:
	//---------------------------------------------------------
	//共通で使用
	//---------------------------------------------------------
	void AddressSet();
	void Create();

	//---------------------------------------------------------
	//サーバーで使用
	//---------------------------------------------------------
	void Bind();
	void Listen();

	//---------------------------------------------------------
	//クライアントで使用
	//---------------------------------------------------------
	void Connect();

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::string ip="localhost";									//IPアドレス
	std::string port="49155";									//ポート番号
	SOCKET m_socket;											//ソケット
	bool is_available = false;
	struct addrinfo *result = NULL, hints;

};
