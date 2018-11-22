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

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	Camera* GetCamera();

private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void State();														//キャラの立っているフラグごとに移動やジャンプの処理を呼び出す
	void Attack();														//攻撃処理
	void Jump();														//ジャンプ処理
	void Wait();														//入力がないとき
	void Move();														//移動
	void Rotation(D3DXVECTOR3 _vec);									//回転
	void ChangeAnimation(int _animation, bool _roop,double _speed=1.0);	//アニメーションの変更
	bool Damage();														//攻撃を受けた
	void TransDataPos(D3DXMATRIX _trans);								//D3DXMATRIXで渡した値を変換し座標にセット
	D3DXMATRIX* CreateMat();											//座標をD3DXMATRIXで渡す


	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	//スキンメッシュ
	CBoneController	skinBoon;
	CMatrix			skinChara;
	CAnimator		skinAnimation;

	//シェーダ
	ToonShader toonShader;

	//Camera* camera;
	int enemyId;
	Data data;
	char jumpFlg = 0;
	D3DXMATRIX initMat;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//加速度
	bool standFlg = false;												//ダメージを受けてから立つまで

};

//static const float MOVESPEEDLIMIT = 0.08;

#endif