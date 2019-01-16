#ifndef Client_h
#define Client_h

class Client
{
public:
	void StartThread();
	bool CreateSocket(std::string _userId,std::string _ip="127.0.0.1");
	void Client::Recv();
	//---------------------------------------------------------
	//送信関数
	//---------------------------------------------------------
	void SendUserInformation(Data* _data);
	void SendPos(Data* _data);
	void SendAttack(std::shared_ptr<Data> _data);

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
	void SetEnemyData(int _enemyid, std::shared_ptr<Data> data);

	//---------------------------------------------------------
	//排他制御
	//---------------------------------------------------------
	void Lock();
	void Unlock();

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static Client & GetInstance();
	static void DeleteInstance();

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
	Client();
	~Client();
	void DataManipulate(char _id,char* _data);							//受信データから移動などの処理を行う	

	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void ExchangeKey(std::string _id);													//鍵交換

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int ATTACK = 11;										//攻撃
	static const int DAMAGE = 13;										//ダメージを受けた
	static const int BYTESIZE = 256;									//送受信に使用するchar配列のデータ量
	static const int ENEMYAMOUNT = 3;
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::unique_ptr<Cipher> cipher = nullptr;														//暗号処理
	std::unique_ptr <ExtensionMutex> mutex = nullptr;												//排他制御
	std::shared_ptr<Socket> socket = nullptr;
	std::shared_ptr <Data> enemyData[ENEMYAMOUNT];													//プレイヤー以外の敵情報を保持
	std::unique_ptr <Data> playerData=nullptr;
	std::shared_ptr <std::queue<Data>> dataQueueList=nullptr;										//完成品データから作成された各情報を保持
	std::vector<char> tempDataList;									//一時的にデータを保存
	std::unique_ptr <std::thread> thread=nullptr;
	bool initFlag=false;
	float count = 0;

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static Client* s_Instance;
	Client & operator=(Client &);										//代入の宣言
	Client(Client&);													//コピーコンストラクタ

};

#define CLIENT Client::GetInstance()

//=========================================================
//以下送信データ構造
//=========================================================
#pragma pack(1)
//データ送信に使う最低限の構造
struct SendData
{
	int size;
	int idSize;
	char id[20];
};
struct BaseData
{
	int size;
	int playerIdSize;
	char playerId[20];
	char id;
};

//座標更新データ
struct PosData
{
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