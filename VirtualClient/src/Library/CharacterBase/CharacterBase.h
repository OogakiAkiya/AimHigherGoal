#ifndef CharacterBase_h
#define CharacterBase_h

struct CharaStatus {
	int hp;							//ヒットポイント
	int mp;							//マジックポイント
	char attack;					//攻撃力
	char defense;					//防御力
	char accurcy;					//命中率
	char movespeed;					//移動速度
};

class CharacterBase {
public:
	virtual ~CharacterBase() {};
	virtual void Update()=0;
	virtual void Render3D() = 0;
	virtual void Render2D() = 0;
	//---------------------------------------------------------
	//情報所得
	//---------------------------------------------------------
	D3DXMATRIX* GetMat();
	CharaStatus* Getstatus();
	std::shared_ptr<Camera> GetCamera();

	//---------------------------------------------------------
	//情報書き込み
	//---------------------------------------------------------
	void SetMat(float _x, float _y, float _z);

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	static const int WAIT = 0;											//待機
	static const int WALK = 1;											//歩く
	static const int RUN = 3;											//走る
	static const int JUMPUP = 7;										//ジャンプ上昇
	static const int JUMPDOWN = 8;										//ジャンプ落下
	static const int LANDING = 9;										//着地
	static const int ATTACK = 11;										//攻撃
	static const int DAMAGE = 13;										//ダメージを受けた
	static const int WAKEUP = 17;
protected:

	//---------------------------------------------------------
	//変数matの値を書き換えるための関数
	//---------------------------------------------------------
	void Trans(float _x, float _y, float _z);
	void RotationLeft(float _rotation);
	void RotationRight(float _rotation);
	void Scale(float _x, float _y, float _z);

	//---------------------------------------------------------
	//判定
	//---------------------------------------------------------
	float RayDecision();
	//---------------------------------------------------------
	//3Dモデルを扱うための行列
	//---------------------------------------------------------
	std::shared_ptr<Camera> camera;
	std::weak_ptr<XFILE> mesh;					//メッシュ
	D3DXMATRIX baseMat;					//キャラクターの行列
	CharaStatus status;				//キャラクターステータス
	
};

#endif