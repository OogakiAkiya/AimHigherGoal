#ifndef Client_h
#define Client_h

class Client
{
public:
	Client();
	~Client();

	void StartThread();
	bool CreateSocket(std::string _ip="127.0.0.1");
	void Client::Recv();
	//---------------------------------------------------------
	//送信関数
	//---------------------------------------------------------
	void SendUserInformation(Data* _data);
	void SendPos(Data* _data);
	void SendAttack(Data* _data);

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	float GetCount();
	SOCKET GetSocket();
	Data GetData();
	Data* GetPlayerData();
	bool GetInitFlg();
	void DeleteData();
	void ClearData();
	bool DataEmpty();
	void SetEnemyData(int _enemyid, Data* data);

	//---------------------------------------------------------
	//排他制御
	//---------------------------------------------------------
	void Lock();
	void Unlock();

	//---------------------------------------------------------
	//ランチャー
	//---------------------------------------------------------
	static void ClientThreadLauncher(void* args) {
		reinterpret_cast<Client*>(args)->Recv();						//無理やりvoid*型にキャストして、本命の処理を実行する。
	}

private:
	//---------------------------------------------------------
	//コンストラクター・デストラクター
	//---------------------------------------------------------
	void DataManipulate(const std::vector<char>* _data);				//完成品データから移動などの処理を行う	

	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void ExchangeKey();													//鍵交換

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int ATTACK = 11;										//攻撃
	static const int DAMAGE = 13;										//ダメージを受けた
	static const int BYTESIZE = 256;									//送受信に使用するchar配列のデータ量

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	Cipher* cipher=nullptr;														//暗号処理
	ExtensionMutex* mutex = nullptr;												//排他制御
	Socket* socket = nullptr;
	std::queue<Data> dataQueueList;										//完成品データから作成された各情報を保持
	std::vector<char> tempDataList;									//一時的にデータを保存
	Data enemyData[3];													//プレイヤー以外の敵情報を保持
	std::shared_ptr<std::thread> thread=nullptr;
	Data* playerData;
	float count = 0;
	bool initFlag = false;

};


//=========================================================
//以下送信データ構造
//=========================================================
#pragma pack(1)
//データ送信に使う最低限の構造
struct BaseData
{
	int size;
	char id;
};

//ユーザー情報確認処理
struct UserData {
	BaseData base;
	int idsize;
	char id[20];
};

//座標更新データ
struct PosData
{
	BaseData base;
	float x;
	float y;
	float z;
	float angle;
	int animation;
};

//攻撃データ
struct AttckData
{
	BaseData base;
	int socket;
};

#pragma pack()

#endif