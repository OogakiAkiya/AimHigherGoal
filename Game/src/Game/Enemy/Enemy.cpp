#include"../../Include.h"
#include"Enemy.h"

Enemy::Enemy(int _id)
{
	//初期座標の設定
	enemyId = _id;
	switch (enemyId) {
	case 0:
		D3DXMatrixTranslation(&initMat, 5.0f, 0.0f, 0.0f);			//初期座標を作成
		break;
	case 1:
		D3DXMatrixTranslation(&initMat, 0.0f, 0.0f, 5.0f);			//初期座標を作成
		break;
	case 2:
		D3DXMatrixTranslation(&initMat, -5.0f, 0.0f, 0.0f);			//初期座標を作成
		break;
	}
	TransDataPos(initMat);											//設定

	// スキンメッシュ読み込み
	SPtr<CSkinMesh> sm = std::make_shared<CSkinMesh>();
	sm->LoadXFile("images/Hackadoll_1/model.x");

	skinBoon.SetSkinMesh(sm);								// メッシュデータをセットし、操作用ボーンを構築
	skinBoon.InitAnimator(skinAnimation);							// このボーン用のアニメータをセットアップ
	skinAnimation.ChangeAnime("立ち", true);						// アニメータにアニメーションを設定


	skinChara.CreateMove(data.GetPos());							//移動
	data.SetAnimation(WAIT);


}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	//立ち上がる処理
	if (standFlg == true) {
		if (skinAnimation.GetAnimePos() > 25 && skinAnimation.GetAnimeNo() == WAKEUP) {
			standFlg = false;
			data.SetAnimation(WAIT);
			if (CLIENT.DataEmpty() == false) {
				CLIENT.Lock();
				CLIENT.ClearData();										//取得データ全削除
				CLIENT.Unlock();
			}
			ChangeAnimation(data.GetAnimation(), true);
			return;
		}
		ChangeAnimation(data.GetAnimation(), false);					//アニメーションの変更
		skinChara.CreateMove(data.GetPos());							//プレイヤーの座標設定
		skinChara.RotateY_Local(data.GetAngle());						//angle分回転
		skinAnimation.Animation(1);										// アニメーションを進行
		skinBoon.CalcBoneMatrix();										// 全ボーン、TransMatを使用しLocalMatを更新する
		return;
	}

	//ダメージを受けていた時
	if (data.GetAnimation() == DAMAGE) {								//ダメージアニメーションの制御
		Damage();
		//ChangeAnimation(data.GetAnimation(), false, 0.8f);
		return;
	}

	//移動
	if (CLIENT.DataEmpty() == false) {
		Data recvdata;												//エネミーのデータ
		D3DXVECTOR3 trans;

		CLIENT.Lock();
		recvdata = CLIENT.GetData();									//データ取得
		CLIENT.Unlock();
		if (recvdata.GetAnimation() == DAMAGE) {					//送られてきたデータがダメージだった場合
			data.SetAnimation(recvdata.GetAnimation());
			acceleration.z = 0.1;
		}
		else {
			trans.x = -recvdata.GetX();							//マイナス余分
			trans.y = recvdata.GetY();
			trans.z = -recvdata.GetZ();							//マイナス余分
			data.SetAngle(recvdata.GetAngle() + 180);			//180度余分
			data.SetAnimation(recvdata.GetAnimation());

			data.SetPos(trans);									//受け取った座標分の移動
			TransDataPos(initMat);								//エネミーの初期座標分だけ移動させる
		}

		//特定のアニメーションの変更処理
		if (data.GetAnimation() == WAIT || data.GetAnimation() == WALK || data.GetAnimation() == RUN) {
			ChangeAnimation(data.GetAnimation(), true,0.8f);
		}
		else {
			ChangeAnimation(data.GetAnimation(), false, 0.8f);
		}
	}

	//クライアントクラスにデータセット
	CLIENT.SetEnemyData(enemyId, &data);

	skinChara.CreateMove(data.GetPos());			//プレイヤーの座標設定
	skinChara.RotateY_Local(data.GetAngle());							//angle分回転
	skinAnimation.Animation(1);									// アニメーションを進行
	skinBoon.CalcBoneMatrix();								// 全ボーン、TransMatを使用しLocalMatを更新する

}


void Enemy::Render2D()
{
}

void Enemy::Render3D()
{
	
	// キャラ(固定機能)
	DEV->SetTransform(D3DTS_WORLD, &skinChara);				//描画する座標設定
	skinBoon.Draw();										//描画
	/*
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
	*/

}

//以下は現在使われていない===========================================================================

Camera* Enemy::GetCamera()
{
	//return camera;
	return nullptr;
}

void Enemy::State()
{

	if (data.GetAnimation() != ATTACK) {															//プレイヤーが攻撃しているならその他の動作をしない
		if (jumpFlg == 0) data.SetAnimation(WAIT);												//何も動作していないなら待機モーションに設定

		if (data.GetAnimation() != JUMPUP&&data.GetAnimation() != JUMPDOWN&&data.GetAnimation() != LANDING) {			//プレイヤーがジャンプしていなかったら移動モーションを使用する
			if (GetAsyncKeyState('W') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
			if (GetAsyncKeyState('A') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
			if (GetAsyncKeyState('S') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
			if (GetAsyncKeyState('D') & 0x8000) data.SetAnimation(WALK);						//移動状態に設定
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000
			&& data.GetAnimation() != JUMPUP
			&& data.GetAnimation() != JUMPDOWN
			&& data.GetAnimation() != LANDING) {
			data.SetAnimation(JUMPUP);															//ジャンプ状態に設定
		}
	}
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && jumpFlg == 0) data.SetAnimation(ATTACK);																//ジャンプしていない状態で左クリックで攻撃

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
	}

	if (data.GetAnimation() == WAIT || data.GetAnimation() == WALK || data.GetAnimation() == RUN) {
		ChangeAnimation(data.GetAnimation(), true);
	}
	else {
		ChangeAnimation(data.GetAnimation(), false);

	}


}

void Enemy::Attack()
{
	data.SetAnimation(ATTACK);												//攻撃モーションの設定

	if (skinAnimation.GetAnimeNo() == ATTACK&&skinAnimation.GetAnimePos() >= 20) {
		CLIENT.SendAttack(&data);														//攻撃が当たっているかどうかの判断と当たっていた場合データを敵に送る
	}

	if (skinAnimation.IsAnimationEnd() == true) data.SetAnimation(WAIT);			//攻撃が終わったら待機モーションに戻る
}

void Enemy::Jump()
{
	if (jumpFlg == 0) {
		jumpFlg = 1;												//1はジャンプ上昇中
		acceleration.y = 0.2f;										//ジャンプする初速設定
		data.SetAnimation(JUMPUP);											//上昇ジャンプ
	}

	if (acceleration.y < 0 && jumpFlg == 1) {
		jumpFlg = 2;												//2はジャンプ下降中
		data.SetAnimation(JUMPDOWN);										//下降
	}

	if (data.GetY() <= 0) {												//キャラクターのy座標が0よりも小さくなった時
		if (jumpFlg == 2) {											//ジャンプ下降中のみ通る
			jumpFlg = 3;											//3は着地状態
			data.SetAnimation(LANDING);									//着地
		}
		data.SetY(0);
	}
	if (jumpFlg == 3) {
		if (skinAnimation.IsAnimationEnd() == true)jumpFlg = 0;			//アニメーションが終わったらジャンプ終了
		return;
	}


	D3DXMATRIX translation;												//移動量分の座標を入れる行列
	D3DXMatrixTranslation(&translation, 0, acceleration.y, 0);			//y軸方向の移動をする
	TransDataPos(translation);											//座標の移動
	acceleration.y -= 0.008;											//加速度を減らす
	Move();																//ジャンプにWASDを押された時の移動処理
}

void Enemy::Wait()
{
	data.SetAnimation(WAIT);																//待機モーション
	acceleration = { 0.001f,0.0f,0.0f };											//加速度初期化
}

void Enemy::Move()
{
	if (data.GetAnimation() == WALK&&skinAnimation.GetAnimeNo() != RUN)data.SetAnimation(WALK);
	//if (data.GetAnimation() == WALK&&acceleration.z > (MOVESPEEDLIMIT - 0.03))data.SetAnimation(RUN);			//一定の速度に達するとアニメーションをする

	D3DXVECTOR3 ToVec(0.0f, 0.0f, 0.0f), tempVec;
	//移動
	if (GetAsyncKeyState('W') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(-1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, -1.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}

	Rotation(ToVec);						//ToVecより移動と回転を行う
}


void Enemy::Rotation(D3DXVECTOR3 _vec)
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
	//if (acceleration.z > MOVESPEEDLIMIT) acceleration.z = MOVESPEEDLIMIT;			//上限値の設定
	if (jumpFlg == 0) {
		/*回転*/
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data.GetAngle()));							//回転用行列に回転量を合成

		D3DXVECTOR3 nowvec;																//①今向いてる方向
		D3DXVec3TransformNormal(&nowvec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &rotation);
		D3DXVec3Normalize(&tovec, &tovec);												//②向きたい方向

																						//①と②を使って内積で角度を出す
		float rotation_angle;
		rotation_angle = D3DXVec3Dot(&nowvec, &tovec);
		rotation_angle = D3DXToDegree(acos(rotation_angle));
		if (rotation_angle >= 0.1f) {													//①と②の外積を求めてそのベクトルが上を向いてたら+下を向いていたら-する
			float nowangle = data.GetAngle();
			D3DXVECTOR3 vCross;															//①と②の外積
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

void Enemy::ChangeAnimation(int _animation, bool _roop, double _speed)
{
	if (skinAnimation.GetAnimeNo() == _animation) return;										//現在のアニメーションとしたいアニメーションが同じなら関数から抜ける
	skinAnimation.ChangeAnimeSmooth(_animation, 0, 10, _roop,_speed);								//アニメーションを_animationに変更

}

bool Enemy::Damage()
{
	if (skinAnimation.GetAnimePos()>40 && data.GetAnimation() == DAMAGE) {
		data.SetAnimation(WAKEUP);
		standFlg = true;
		if (CLIENT.DataEmpty() == false) {
			CLIENT.Lock();
			CLIENT.ClearData();										//取得データ全削除
			CLIENT.Unlock();
		}
		return false;
	}

	D3DXMATRIX translation;												//移動量分の座標を入れる行列
	D3DXMatrixTranslation(&translation, 0, 0,acceleration.z);			//z軸方向の移動をする
	TransDataPos(translation);
	acceleration.z -= 0.003;											//加速度を減らす
	if (acceleration.z < 0)acceleration.z = 0.0f;

	skinChara.CreateMove(data.GetPos());			//プレイヤーの座標設定
	skinChara.RotateY_Local(data.GetAngle());							//angle分回転
	skinAnimation.Animation(1);									// アニメーションを進行
	skinBoon.CalcBoneMatrix();								// 全ボーン、TransMatを使用しLocalMatを更新する
	return true;
}


void Enemy::TransDataPos(D3DXMATRIX _trans)
{
	D3DXVECTOR3 transpos(_trans._41, _trans._42, _trans._43);
	D3DXVECTOR3 pos(data.GetX(), data.GetY(), data.GetZ());
	pos += transpos;
	data.SetPos(pos);
}

D3DXMATRIX* Enemy::CreateMat()
{
	D3DXMATRIX returnmat;
	D3DXMatrixTranslation(&returnmat, data.GetX(), data.GetY(), data.GetZ());
	return &returnmat;
}
