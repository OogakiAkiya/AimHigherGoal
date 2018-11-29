#include"../../Include.h"
#include"Player.h"

Player::Player()
{
	//インスタンスの生成
	camera = new Camera();									//カメラ設定
	data = new Data();

	// スキンメッシュ読み込み
	SPtr<CSkinMesh> sm = std::make_shared<CSkinMesh>();
	sm->LoadXFile("images/Hackadoll_1/model.x");
	skinBone.SetSkinMesh(sm);								// メッシュデータをセットし、操作用ボーンを構築
	skinBone.InitAnimator(skinAnimation);					// このボーン用のアニメータをセットアップ
	skinAnimation.ChangeAnime("立ち", true);				// アニメータにアニメーションを設定
	skinChara.CreateMove(0, 0, 0);							//移動
	skinChara.RotateY_Local(0);								//回転
	data->SetAnimation(WAIT);								//アニメーション作成

	//シェーダー読み込み
	toonShader.Init();

	//ユーザーデータ送信
	data->SetId("test");
	CLIENT.SendUserInformation(data);
}

Player::~Player()
{
	//解放処理
	toonShader.Release();
	delete camera;
	delete data;
}

void Player::Update()
{
	//更新処理
	State();												//プレイヤーの状態をチェックしその処理を実行する
	D3DXMATRIX mat=*CreateMat();
	camera->SetMat(mat);									//カメラにPlayerの行列を渡す
	camera->ComplianceUpdate();								//追従カメラを起動
	skinChara.CreateMove(data->GetPos());					//プレイヤーの座標設定
	skinChara.RotateY_Local(data->GetAngle());				//angle分回転
	skinAnimation.Animation(1);								// アニメーションを進行
	skinBone.CalcBoneMatrix();								// 全ボーン、TransMatを使用しLocalMatを更新する

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
	toonShader.SetTransformProj(&camera->GetProj());
	// シェーダ側のグローバル定数にビュー行列をセット
	toonShader.SetTransformView(&camera->GetView());

	// ビュー行列からカメラ行列に変換する

	// 固定機能のライト情報を取得し、シェーダ側のグローバル定数にライト情報をセット
	D3DLIGHT9 light = DX.GetLight();	// 固定機能のライト情報を取得
	toonShader.SetDirectionalLight(&light);

	// シェーダ側のグローバル定数にカメラ座標をセット
	toonShader.SetCamPos(&D3DXVECTOR3(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z));
	toonShader.SetTransformWorld(&skinChara);	// シェーダ側のグローバル定数にワールド行列をセットする
	toonShader.DrawMesh(&skinBone);			// シェーダで描画
	

}


void Player::State()
{
	//ダメージを受ける処理(デバック用)
	if (GetAsyncKeyState('Q') & 0x8000&&data->GetAnimation()!=DAMAGE) {
		data->SetAnimation(DAMAGE);
		acceleration.z = -0.17;
	}

	//各判定処理
	if (data->GetAnimation() != WAKEUP) {
		if (data->GetAnimation() != DAMAGE) {															//ダメージを受けていたらその他の動作をしない
			if (data->GetAnimation() != ATTACK) {														//プレイヤーが攻撃しているならその他の動作をしない
				if (jumpFlg == 0) data->SetAnimation(WAIT);												//何も動作していないなら待機モーションに設定

				if (data->GetAnimation() != JUMPUP&&data->GetAnimation() != JUMPDOWN&&data->GetAnimation() != LANDING) {			//プレイヤーがジャンプしていなかったら移動モーションを使用する
					if (!GetAsyncKeyState('S') &&GetAsyncKeyState('W') & 0x8000) data->SetAnimation(WALK);						//移動状態に設定
					if (!GetAsyncKeyState('D') && GetAsyncKeyState('A') & 0x8000) data->SetAnimation(WALK);						//移動状態に設定
					if (!GetAsyncKeyState('W') &&GetAsyncKeyState('S') & 0x8000) data->SetAnimation(WALK);						//移動状態に設定
					if (!GetAsyncKeyState('A') && GetAsyncKeyState('D') & 0x8000) data->SetAnimation(WALK);						//移動状態に設定
				}

				if (GetAsyncKeyState(VK_SPACE) & 0x8000
					&& data->GetAnimation() != JUMPUP
					&& data->GetAnimation() != JUMPDOWN
					&& data->GetAnimation() != LANDING) {
					data->SetAnimation(JUMPUP);															//ジャンプ状態に設定
				}
			}
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && jumpFlg == 0) data->SetAnimation(ATTACK);																//ジャンプしていない状態で左クリックで攻撃
		}
	}

	//アニメーションにより処理を決定する
	switch (data->GetAnimation()) {
	case WAIT:							//待機モーション
		Wait();
		break;
	case WALK:							//移動モーション
		Move();
		break;
	case JUMPUP:						//ジャンプモーション(上昇)
	case JUMPDOWN:						//ジャンプモーション(落下)
	case LANDING:						//着地
		Jump();
		break;
	case ATTACK:						//攻撃モーション
		Attack();
		break;
	case DAMAGE:						//ダメージ
		Damage();
		break;
	case WAKEUP:						//起き上がる
		WakeUp();
		break;

	}

	//特定の処理の時のみループアニメーションを設定
	if (data->GetAnimation() == WAIT || data->GetAnimation() == WALK || data->GetAnimation() == RUN) {
		ChangeAnimation(data->GetAnimation(), true);
	}
	else {
		ChangeAnimation(data->GetAnimation(), false);

	}

	//プレイヤーの座標を常に送信
	CLIENT.SendPos(data);											//サーバーに座標を送信

}

void Player::Attack()
{
	//攻撃処理
	if (skinAnimation.GetAnimeNo() == ATTACK&&skinAnimation.GetAnimePos()== 20) {
		CLIENT.SendAttack(data);													//攻撃が当たっているかどうかの判断と当たっていた場合データを敵に送る
	}

	if (skinAnimation.IsAnimationEnd() == true) data->SetAnimation(WAIT);			//攻撃が終わったら待機モーションに戻る
}

void Player::Jump()
{
	//ジャンプ処理
	if (jumpFlg == JUMPRESET) {
		jumpFlg = JUMPUPER;											//ジャンプ上昇中
		acceleration.y = 0.2f;										//ジャンプする初速設定
		data->SetAnimation(JUMPUP);									//上昇ジャンプ
	}

	//下降中の処理
	if (acceleration.y < 0&&jumpFlg==1) {
		jumpFlg = JUMPDOWNER;										//ジャンプ下降中
		data->SetAnimation(JUMPDOWN);								//下降
	}

	//着地処理
	if (data->GetY() <= 0) {											//キャラクターのy座標が0よりも小さくなった時
		if (jumpFlg == JUMPDOWNER) {								//ジャンプ下降中のみ通る
			jumpFlg = JUMPSTANDING;									//3は着地状態
			data->SetAnimation(LANDING);								//着地
		}
		data->SetY(0);
	}

	//ジャンプ処理終了
	if (jumpFlg == JUMPSTANDING) {
		if (skinAnimation.IsAnimationEnd() == true)jumpFlg = JUMPRESET;		//アニメーションが終わったらジャンプ終了
		return;
	}

	D3DXVECTOR3 vec(0, acceleration.y, 0);
	TransDataPos(&vec);												//座標の移動
	acceleration.y -= 0.008;										//加速度を減らす
	Move();															//ジャンプにWASDを押された時の移動処理
}

void Player::Wait()
{
	data->SetAnimation(WAIT);										//待機モーション
	acceleration = { 0.001f,0.0f,0.0f };							//加速度初期化
}

void Player::Move()
{
	//歩くか走るかを判定
	if (data->GetAnimation() == WALK&&skinAnimation.GetAnimeNo() != RUN)data->SetAnimation(WALK);
	if (data->GetAnimation() == WALK&&acceleration.z > (MOVESPEEDLIMIT - 0.03))data->SetAnimation(RUN);			//一定の速度に達するとダッシュモーションをする

	D3DXVECTOR3 toVec(0.0f, 0.0f, 0.0f), tempVec;

	//移動
	if (GetAsyncKeyState('W') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &camera->GetRotMat());
		toVec += tempVec;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		toVec += tempVec;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(-1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		toVec += tempVec;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, -1.0f), &camera->GetRotMat());
		toVec += tempVec;
	}

	Rotation(toVec);						//ToVecより移動と回転を行う
}

void Player::Damage()
{
	//アニメーション設定
	if (skinAnimation.GetAnimePos() >= 55 && data->GetAnimation() == DAMAGE) data->SetAnimation(WAKEUP);			//ダメージアニメーションが一定の位置まで来ると
																												//起き上がるモーションに変更する
	ChangeAnimation(data->GetAnimation(), false);
	
	//後退処理
	D3DXVECTOR3 vec=*GetVector(0,0,acceleration.z);
	TransDataPos(&vec);
	acceleration.z += 0.005f;											//加速度を減らす
	if (acceleration.z > 0)acceleration.z = 0.0f;

}

void Player::WakeUp()
{
	//アニメーション設定
	if (skinAnimation.GetAnimePos() == 26 && data->GetAnimation() == WAKEUP) data->SetAnimation(WAIT);

	//前進処理
	D3DXVECTOR3 vec=*GetVector(0.0f, 0.0f, 0.025f);
	TransDataPos(&vec);
	ChangeAnimation(data->GetAnimation(), false);
}


void Player::Rotation(D3DXVECTOR3 _vec)
{
	//ローカル変数
	D3DXVECTOR3 toVec = _vec;
	D3DXMATRIX trans;																//移動用行列
	D3DXMATRIX rotation;															//回転用行列

	//初期化
	D3DXMatrixIdentity(&rotation);													//単位行列に初期化
	D3DXMatrixIdentity(&trans);														//単位行列に初期化

	//移動
	D3DXMATRIX moveMat;
	D3DXVec3Normalize(&toVec, &toVec);												//正規化
	toVec *= acceleration.z;														//移動量を設定
	D3DXMatrixTranslation(&moveMat, toVec.x, toVec.y, toVec.z);						//行列の作成
	trans = moveMat*trans;															//行列の作成
	acceleration.z += 0.0005;														//移動用加速変数の値を足しこむ
	if (acceleration.z > MOVESPEEDLIMIT) acceleration.z = MOVESPEEDLIMIT;			//上限値の設定
	if (jumpFlg == 0) {
		//回転
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data->GetAngle()));				//回転用行列に回転量を合成

		D3DXVECTOR3 nowVec;															//①今向いてる方向
		D3DXVec3TransformNormal(&nowVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &rotation);
		D3DXVec3Normalize(&toVec, &toVec);											//②向きたい方向

		//①と②を使って内積で角度を出す
		float rotationAngle;
		rotationAngle = D3DXVec3Dot(&nowVec, &toVec);
		rotationAngle = D3DXToDegree(acos(rotationAngle));
		if (rotationAngle >= 0.1f) {												//①と②の外積を求めてそのベクトルが上を向いてたら+下を向いていたら-する
			float nowAngle = data->GetAngle();
			D3DXVECTOR3 vCross;														//①と②の外積
			D3DXVec3Cross(&vCross, &nowVec, &toVec);								//外積
			D3DXVec3Normalize(&vCross, &vCross);									//正規化

			if (rotationAngle >= 10.0f) { rotationAngle = 5.0f; }					//いきなり曲がりすぎないように少しずつ移動するようにする
			if (vCross.y >= 0.9) {													//ほぼ1である
				nowAngle += rotationAngle;
			}
			else if (vCross.y <= -0.9f) {											//ほぼ-1である
				nowAngle -= rotationAngle;
			}
			else {
				nowAngle += rotationAngle;											//真後ろつまり真逆ベクトルを見られたときは足しても引いてもよい
			}
			data->SetAngle(nowAngle);
		}
		//回転
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data->GetAngle()));
	}
	TransDataPos(trans);

	//SOCK.SendPos(&posdata);														//移動時のみサーバーに座標を送信

}

void Player::ChangeAnimation(int _animation,bool _roop,float _speed)
{
	if (skinAnimation.GetAnimeNo() == _animation) return;							//現在のアニメーションとしたいアニメーションが同じなら関数から抜ける
		skinAnimation.ChangeAnimeSmooth(_animation, 0, 10, _roop,_speed);			//アニメーションを_animationに変更
}

void Player::TransDataPos(D3DXMATRIX _trans)
{
	D3DXVECTOR3 transpos(_trans._41, _trans._42, _trans._43);
	D3DXVECTOR3 pos(data->GetX(),data->GetY(),data->GetZ());
	pos += transpos;
	data->SetPos(pos);
}

void Player::TransDataPos(D3DXVECTOR3* _vec)
{
	D3DXVECTOR3 transpos(_vec->x, _vec->y, _vec->z);
	D3DXVECTOR3 pos(data->GetX(), data->GetY(), data->GetZ());
	pos += transpos;
	data->SetPos(pos);
}


D3DXMATRIX* Player::CreateMat()
{
	D3DXMATRIX returnmat;
	D3DXMatrixTranslation(&returnmat, data->GetX(), data->GetY(), data->GetZ());
	return &returnmat;
}

D3DXVECTOR3 * Player::GetVector(float _x,float _y,float _z)
{
	D3DXVECTOR3 vec;													//プレイヤーの見ている方向から引数分だけ移動した値を入れる
	D3DXMATRIX rotmat;													//プレイヤーの回転
	D3DXMatrixIdentity(&rotmat);
	D3DXMatrixRotationY(&rotmat, D3DXToRadian(data->GetAngle()));		//左右に回転させる
	D3DXVec3TransformNormal(&vec, &D3DXVECTOR3(_x, _y, _z), &rotmat);

	return &vec;
}
