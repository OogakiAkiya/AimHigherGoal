#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"Ground.h"
#include"../Game/Enemy/Enemy.h"
#include"Player/Player.h"
#include"GameMain.h"


GameMain::GameMain()
{
	feald = new Ground();						//地面作成
	CharacterBase* temp = new Player();
	characterlist.push_back(temp);
	for (int i = 0;i<3; i++) {
		temp = new Enemy(i);
		characterlist.push_back(temp);
	}

	//camera = player->GetCamera();				//Playerのカメラを取得する
	camera = characterlist.at(0)->GetCamera();
	ShowCursor(FALSE);

	//空(一時的に作成)
	D3DXMatrixTranslation(&s_mat, 0.0f, 0.0f, 1.0f);
	s_mesh=CHECKMESH.Get("images/Sky.x");
}

GameMain::~GameMain()
{
	delete feald;								//地面削除
	CharacterBase* deletechara;					//キャラ削除
	for (auto element : characterlist) {
		deletechara = element;
		delete deletechara;
	}
	characterlist.erase(characterlist.begin(), characterlist.end());
}

void GameMain::Update()
{
	MOUSE.ThirdPersonPointUpdata();							//マウスの座標更新
	for (auto element : characterlist) {
		element->Update();
	}
	//クライアントに送られてきているデータ削除(ここにあるのはエネミー全体に動作を反映させるため)
	if (CLIENT.DataEmpty() == false) {
		CLIENT.Lock();
		CLIENT.DeleteData();											//取得データ削除
		CLIENT.Unlock();
	}
	camera = characterlist.at(0)->GetCamera();
}

void GameMain::Render3D()
{
	for (auto element : characterlist) {
		element->Render3D();
	}

	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//ライトOFF
	feald->Draw();										//地面描画
	s_mesh->Draw(DEV, &s_mat);							//空描画(一時的)
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//ライトON

}


void GameMain::Render2D()
{
	//2D描画
	for (auto element : characterlist) {
		element->Render2D();
	}

}

