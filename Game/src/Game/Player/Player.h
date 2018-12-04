#ifndef Player_h
#define Player_h

class Player :public CharacterBase
{
public:
	Player();
	~Player() override;
	virtual void Update() override;
	virtual void Render2D() override;
	virtual void Render3D() override;
	
private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void State();														//キャラの立っているフラグごとに移動やジャンプの処理を呼び出す
	void Attack();														//攻撃処理
	void Jump();														//ジャンプ処理
	void Wait();														//入力がないとき
	void Move();														//移動
	void Damage();														//ダメージ	
	void WakeUp();														//起き上がり
	void Rotation(D3DXVECTOR3 _vec);									//回転
	void ChangeAnimation(int _animation, bool _roop,float _speed=1.0f);	//アニメーションの変更
	void TransDataPos(D3DXMATRIX _trans);								//座標セット
	void TransDataPos(D3DXVECTOR3* _vec);								//座標セット
	D3DXMATRIX* CreateMat();											//座標出力
	D3DXVECTOR3* GetVector(float _x, float _y, float _z);				//プレイヤーの向いている方向から引数方向に移動したときのベクトルを求める

	//---------------------------------------------------------
	//定数
	//---------------------------------------------------------
	//将来的になくなる
	static const int JUMPRESET = 0;
	static const int JUMPUPER = 1;
	static const int JUMPDOWNER = 2;
	static const int JUMPSTANDING = 3;
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	//スキンメッシュ
	CBoneController	skinBone;
	CMatrix			skinChara;
	CAnimator		skinAnimation;

	//シェーダ
	ToonShader toonShader;

	Data* data;
	char jumpFlg = 0;
	bool attackFlg = false;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//加速度
};

static const float MOVESPEEDLIMIT = 0.08;
#endif