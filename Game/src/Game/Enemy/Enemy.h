#ifndef Enemy_h
#define Enemy_h

class Enemy :public CharacterBase
{
public:
	Enemy(int _id);
	~Enemy();
	void Update();
	void Render2D();
	void Render3D();
	Camera* GetCamera();
private:	
	void State();														//キャラの立っているフラグごとに移動やジャンプの処理を呼び出す
	void Attack();														//攻撃処理
	void Jump();														//ジャンプ処理
	void Wait();														//入力がないとき
	void Move();														//移動
	void Rotation(D3DXVECTOR3 _vec);									//回転
	void ChangeAnimation(int _animation, bool _roop,double _speed=1.0);
	bool Damage();
	void TransDataPos(D3DXMATRIX _trans);
	D3DXMATRIX* CreateMat();


	//スキンメッシュ
	CBoneController	m_bcChara;
	CMatrix			m_mChara;
	CAnimator		m_anim;

	//シェーダ
	ToonShader m_toonshader;

	//Camera* camera;
	int enemyid;
	Data data;
	char jumpflg = 0;
	D3DXMATRIX initmat;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//加速度
	bool standflg = false;												//ダメージを受けてから立つまで

};

//static const float MOVESPEEDLIMIT = 0.08;

#endif