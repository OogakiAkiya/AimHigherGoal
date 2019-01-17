#ifndef Client_h
#define Client_h

struct Header;

class Client
{
public:
	Client();
	~Client();
	void Update();

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	SOCKET GetSocket();
	std::shared_ptr<Data> GetData();
	OpenSSLAES* GetAES();
	void SetPosGetFlg();
	bool GetPosGetFlg();

	std::queue<NamedPipe::PipeData>* GetSendData();
	void AddData(NamedPipe::PipeData* _data);

private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void DataProcess(Header* _header);
	void DBGetPos(char* _data, std::shared_ptr<std::string> _userId);
	void CreateSendData(char* _originalData, int _dataLen, char _id);
	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int BYTESIZE = 516;
	static const int EXCHANGEKEY = 1;
	static const int CREATECOMPLETEDATA = 2;

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	bool posGetFlg = false;
	std::shared_ptr<Data> data;												//ユーザーの各データが格納されている
	std::unique_ptr<OpenSSLAES> aes;										//共通鍵暗号クラス
	std::unique_ptr<std::queue<NamedPipe::PipeData>> tempDataQueue;			//デコード前のデータ
	std::unique_ptr<std::queue<NamedPipe::PipeData>> completeDataQueue;		//送信データ


};

#pragma pack(1)
struct Header
{
	int size;
	int playerIdSize;
	char playerId[20];
	unsigned char id;
};
struct Pos
{
	float x;
	float y;
	float z;
};

//座標データ
struct UserData {
	float x;
	float y;
	float z;
	float angle;
	int animation;
};

#pragma pack()


#endif