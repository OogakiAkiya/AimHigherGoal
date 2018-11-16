#include"../../Include.h"
#include"Player.h"

Player::Player()
{
	camera = new Camera();									//カメラ設定

	// スキンメッシュ読み込み
	SPtr<CSkinMesh> sm = std::make_shared<CSkinMesh>();
	sm->LoadXFile("images/Hackadoll_1/model.x");

	m_bcChara.SetSkinMesh(sm);								// メッシュデータをセットし、操作用ボーンを構築
	m_bcChara.InitAnimator(m_anim);							// このボーン用のアニメータをセットアップ
	m_anim.ChangeAnime("立ち", true);						// アニメータにアニメーションを設定

	m_mChara.CreateMove(0, 0, 0);							//移動
	m_mChara.RotateY_Local(0);								//回転
	data.SetAnimation(WAIT);

	m_toonshader.Init();

}

Player::~Player()
{
	m_toonshader.Release();
	delete camera;
}

void Player::Update()
{

	State();												//プレイヤーの状態をチェックしその処理を実行する
	D3DXMATRIX mat=*CreateMat();
	camera->SetMat(mat);									//カメラにPlayerの行列を渡す
	camera->ComplianceUpdate();								//追従カメラを起動

	m_mChara.CreateMove(data.GetPos());			//プレイヤーの座標設定
	m_mChara.RotateY_Local(data.GetAngle());							//angle分回転
	m_anim.Animation(1);									// アニメーションを進行
	m_bcChara.CalcBoneMatrix();								// 全ボーン、TransMatを使用しLocalMatを更新する

}


void Player::Render2D()
{
}

void Player::Render3D()
{
	// 固定機能から、ビュー行列と射影行列を取得
	DEV->GetTransform(D3DTS_VIEW, &camera->GetView());
	DEV->GetTransform(D3DTS_PROJECTION, &camera->GetProj());
	// シェーダ側のグローバル定数に射影行列をセット
	m_toonshader.SetTransformProj(&camera->GetProj());
	// シェーダ側のグローバル定数にビュー行列をセット
	m_toonshader.SetTransformView(&camera->GetView());

	// ビュー行列からカメラ行列に変換する

	// 固定機能のライト情報を取得し、シェーダ側のグローバル定数にライト情報をセット
	D3DLIGHT9 light = DX.GetLight();	// 固定機能のライト情報を取得
	m_toonshader.SetDirectionalLight(&light);

	// シェーダ側のグローバル定数にカメラ座標をセット
	m_toonshader.SetCamPos(&D3DXVECTOR3(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z));
	m_toonshader.SetTransformWorld(&m_mChara);	// シェーダ側のグローバル定数にワールド行列をセットする
	m_toonshader.DrawMesh(&m_bcChara);			// シェーダで描画
	

}


void Player::State()
{
	if (GetAsyncKeyState('Q') & 0x8000&&data.GetAnimation()!=DAMAGE) {
		data.SetAnimation(DAMAGE);
		acceleration.z = -0.17;
	}

	if (data.GetAnimation() != WAKEUP) {
		if (data.GetAnimation() != DAMAGE) {															//ダメージを受けていたらその他の動作をしない
			if (data.GetAnimation() != ATTACK) {														//プレイヤーが攻撃しているならその他の動作をしない
				if (jumpflg == 0) data.SetAnimation(WAIT);												//何も動作していないなら待機モーションに設定

				if (data.GetAnimation() != JUMPUP&&data.GetAnimation() != JUMPDOWN&&data.GetAnimation() != LANDING) {			//プレイヤーがジャンプしていなかったら移動モーションを使用する
					if (!GetAsyncKeyState('S') &&GetAsyncKeyState('W') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
					if (!GetAsyncKeyState('D') && GetAsyncKeyState('A') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
					if (!GetAsyncKeyState('W') &&GetAsyncKeyState('S') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
					if (!GetAsyncKeyState('A') && GetAsyncKeyState('D') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
				}

				if (GetAsyncKeyState(VK_SPACE) & 0x8000
					&& data.GetAnimation() != JUMPUP
					&& data.GetAnimation() != JUMPDOWN
					&& data.GetAnimation() != LANDING) {
					data.SetAnimation(JUMPUP);															//ジャンプ状態に設定
				}
			}
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && jumpflg == 0) data.SetAnimation(ATTACK);																//ジャンプしていない状態で左クリックで攻撃
		}
	}

	switch (data.GetAnimation()) {
	case WAIT:							//待機モーション
		Wait();
		break;
	case WALK:							//移動モーション
		Move();
		break;
	case JUMPUP:						//ジャンプモーション
	case JUMPDOWN:
	case LANDING:
		Jump();
		break;
	case ATTACK:						//攻撃モーション
		Attack();
		break;
	case DAMAGE:
		Damage();
		break;
	case WAKEUP:
		WakeUp();
		break;

	}

	if (data.GetAnimation() == WAIT || data.GetAnimation() == WALK || data.GetAnimation() == RUN) {
		ChangeAnimation(data.GetAnimation(), true);
	}
	else {
		ChangeAnimation(data.GetAnimation(), false);

	}

	//プレイヤーの座標を常に送信
	CLIENT.SendPos(&data);											//サーバーに座標を送信

}

void Player::Attack()
{
	data.SetAnimation(ATTACK);												//攻撃モーションの設定

	if (m_anim.GetAnimeNo() == ATTACK&&m_anim.GetAnimePos()== 20) {
		CLIENT.SendAttack(&data);														//攻撃が当たっているかどうかの判断と当たっていた場合データを敵に送る
	}

	if (m_anim.IsAnimationEnd() == true) data.SetAnimation(WAIT);			//攻撃が終わったら待機モーションに戻る
}

void Player::Jump()
{
	if (jumpflg == 0) {
		jumpflg = 1;												//1はジャンプ上昇中
		acceleration.y = 0.2f;										//ジャンプする初速設定
		data.SetAnimation(JUMPUP);											//上昇ジャンプ
	}

	if (acceleration.y < 0&&jumpflg==1) {
		jumpflg = 2;												//2はジャンプ下降中
		data.SetAnimation(JUMPDOWN);										//下降
	}

	if (data.GetY() <= 0) {												//キャラクターのy座標が0よりも小さくなった時
		if (jumpflg == 2) {											//ジャンプ下降中のみ通る
			jumpflg = 3;											//3は着地状態
			data.SetAnimation(LANDING);									//着地
		}
		data.SetY(0);
	}
	if (jumpflg == 3) {
		if (m_anim.IsAnimationEnd() == true)jumpflg = 0;			//アニメーションが終わったらジャンプ終了
		return;
	}

	D3DXVECTOR3 vec(0, acceleration.y, 0);
	TransDataPos(&vec);											//座標の移動
	acceleration.y -= 0.008;											//加速度を減らす
	Move();																//ジャンプにWASDを押された時の移動処理
}

void Player::Wait()
{
	data.SetAnimation(WAIT);																//待機モーション
	acceleration = { 0.001f,0.0f,0.0f };											//加速度初期化
}

void Player::Move()
{
	if (data.GetAnimation() == WALK&&m_anim.GetAnimeNo() != RUN)data.SetAnimation(WALK);
	if (data.GetAnimation() == WALK&&acceleration.z > (MOVESPEEDLIMIT - 0.03))data.SetAnimation(RUN);			//一定の速度に達するとアニメーションをする

	D3DXVECTOR3 ToVec(0.0f, 0.0f, 0.0f), tempVec;
	//移動
	if (GetAsyncKeyState('W') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(-1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, -1.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}

	Rotation(ToVec);						//ToVecより移動と回転を行う
}

void Player::Damage()
{
	if (m_anim.GetAnimePos() >= 55 && data.GetAnimation() == DAMAGE) data.SetAnimation(WAKEUP);
	ChangeAnimation(data.GetAnimation(), false);
	
	D3DXVECTOR3 vec=*GetVector(0,0,acceleration.z);
	TransDataPos(&vec);
	acceleration.z += 0.005f;											//加速度を減らす
	if (acceleration.z > 0)acceleration.z = 0.0f;

}

void Player::WakeUp()
{
	if (m_anim.GetAnimePos() == 26 && data.GetAnimation() == WAKEUP) data.SetAnimation(WAIT);
	D3DXVECTOR3 vec=*GetVector(0.0f, 0.0f, 0.025f);
	TransDataPos(&vec);
	ChangeAnimation(data.GetAnimation(), false);
}


void Player::Rotation(D3DXVECTOR3 _vec)
{
	/*ローカル変数*/
	D3DXVECTOR3 tovec = _vec;
	D3DXMATRIX trans;																//移動用行列
	D3DXMATRIX rotation;															//回転用行列

	/*初期化*/
	D3DXMatrixIdentity(&rotation);													//単位行列に初期化
	D3DXMatrixIdentity(&trans);														//単位行列に初期化

	/*移動*/
	D3DXMATRIX move_mat;
	D3DXVec3Normalize(&tovec, &tovec);												//正規化
	tovec *= acceleration.z;														//移動量を設定
	D3DXMatrixTranslation(&move_mat, tovec.x, tovec.y, tovec.z);						//行列の作成
	trans = move_mat*trans;															//行列の作成
	acceleration.z += 0.0005;														//移動用加速変数の値を足しこむ
	if (acceleration.z > MOVESPEEDLIMIT) acceleration.z = MOVESPEEDLIMIT;			//上限値の設定
	if (jumpflg == 0) {
		/*回転*/
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data.GetAngle()));							//回転用行列に回転量を合成

		D3DXVECTOR3 nowvec;																//�@今向いてる方向
		D3DXVec3TransformNormal(&nowvec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &rotation);
		D3DXVec3Normalize(&tovec, &tovec);												//�A向きたい方向

		//�@と�Aを使って内積で角度を出す
		float rotation_angle;
		rotation_angle = D3DXVec3Dot(&nowvec, &tovec);
		rotation_angle = D3DXToDegree(acos(rotation_angle));
		if (rotation_angle >= 0.1f) {													//�@と�Aの外積を求めてそのベクトルが上を向いてたら+下を向いていたら-する
			float nowangle = data.GetAngle();
			D3DXVECTOR3 vCross;															//�@と�Aの外積
			D3DXVec3Cross(&vCross, &nowvec, &tovec);									//外積
			D3DXVec3Normalize(&vCross, &vCross);										//正規化

			if (rotation_angle >= 10.0f) { rotation_angle = 5.0f; }						//いきなり曲がりすぎないように少しずつ移動するようにする
			if (vCross.y >= 0.9) {														//ほぼ1である
				nowangle += rotation_angle;
			}
			else if (vCross.y <= -0.9f) {												//ほぼ-1である
				nowangle -= rotation_angle;
			}
			else {
				nowangle += rotation_angle;												//真後ろつまり真逆ベクトルを見られたときは足しても引いてもよい
			}
			data.SetAngle(nowangle);
		}
		/*回転*/
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data.GetAngle()));
	}
	TransDataPos(trans);

	//SOCK.SendPos(&posdata);																			//移動時のみサーバーに座標を送信

}

void Player::ChangeAnimation(int _animation,bool _roop,float _speed)
{
	if (m_anim.GetAnimeNo() == _animation) return;										//現在のアニメーションとしたいアニメーションが同じなら関数から抜ける
		m_anim.ChangeAnimeSmooth(_animation, 0, 10, _roop,_speed);								//アニメーションを_animationに変更
}

void Player::TransDataPos(D3DXMATRIX _trans)
{
	D3DXVECTOR3 transpos(_trans._41, _trans._42, _trans._43);
	D3DXVECTOR3 pos(data.GetX(),data.GetY(),data.GetZ());
	pos += transpos;
	data.SetPos(pos);
}

void Player::TransDataPos(D3DXVECTOR3* _vec)
{
	D3DXVECTOR3 transpos(_vec->x, _vec->y, _vec->z);
	D3DXVECTOR3 pos(data.GetX(), data.GetY(), data.GetZ());
	pos += transpos;
	data.SetPos(pos);
}


D3DXMATRIX* Player::CreateMat()
{
	D3DXMATRIX returnmat;
	D3DXMatrixTranslation(&returnmat, data.GetX(), data.GetY(), data.GetZ());
	return &returnmat;
}

D3DXVECTOR3 * Player::GetVector(float _x,float _y,float _z)
{
	D3DXVECTOR3 vec;													//プレイヤーの見ている方向から引数分だけ移動した値を入れる
	D3DXMATRIX rotmat;													//プレイヤーの回転
	D3DXMatrixIdentity(&rotmat);
	D3DXMatrixRotationY(&rotmat, D3DXToRadian(data.GetAngle()));		//左右に回転させる
	D3DXVec3TransformNormal(&vec, &D3DXVECTOR3(_x, _y, _z), &rotmat);

	return &vec;
}
