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
	void State();														//キャラの立っているフラグごとに移動やジャンプの処理を呼び出す
	void Attack();														//攻撃処理
	void Jump();														//ジャンプ処理
	void Wait();														//入力がないとき
	void Move();														//移動
	void Damage();
	void WakeUp();
	void Rotation(D3DXVECTOR3 _vec);									//回転
	void ChangeAnimation(int _animation, bool _roop,float _speed=1.0);
	void TransDataPos(D3DXMATRIX _trans);
	void TransDataPos(D3DXVECTOR3* _vec);

	D3DXMATRIX* CreateMat();
	D3DXVECTOR3* GetVector(float _x, float _y, float _z);				//プレイヤーの向いている方向から引数方向に移動したときのベクトルを求める

	//スキンメッシュ
	CBoneController	m_bcChara;
	CMatrix			m_mChara;
	CAnimator		m_anim;

	//シェーダ
	ToonShader m_toonshader;
	Data data;
	char jumpflg = 0;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//加速度
};

static const float MOVESPEEDLIMIT = 0.08;
#endif