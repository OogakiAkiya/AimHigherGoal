#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"Ground.h"
#include"../Game/Enemy/Enemy.h"
#include"Player/Player.h"
#include"GameMain.h"


GameMain::GameMain()
{
	//マップ生成
	feald = new Ground();

	//キャラ生成
	CharacterBase* temp = new Player();
	characterList.push_back(temp);

	for (int i = 0;i<3; i++) {
		temp = new Enemy(i);
		characterList.push_back(temp);
	}

	//カメラ設定
	camera = characterList.at(0)->GetCamera();

	//マウスポインタ非表示
	ShowCursor(FALSE);

	//空(一時的に作成)
	D3DXMatrixTranslation(&s_mat, 0.0f, 0.0f, 1.0f);
	s_mesh=CHECKMESH.Get("images/Sky.x");
}

GameMain::~GameMain()
{
	//解放処理
	delete feald;								//地面削除
	CharacterBase* deleteChara;					//キャラ削除
	for (auto element : characterList) {
		deleteChara = element;
		delete deleteChara;
	}
	characterList.erase(characterList.begin(), characterList.end());
}

void GameMain::Update()
{
	//マウスポインタ座標更新処理
	MOUSE.ThirdPersonPointUpdata();

	//キャラの更新処理
	for (auto element : characterList) {
		element->Update();
	}

	//クライアントに送られてきているデータ削除(ここにあるのはエネミー全体に動作を反映させるため)
	if (CLIENT.DataEmpty() == false) {
		CLIENT.Lock();
		CLIENT.DeleteData();											//取得データ削除
		CLIENT.Unlock();
	}

	//カメラセット
	camera = characterList.at(0)->GetCamera();
}

void GameMain::Render3D()
{
	//キャラ描画
	for (auto element : characterList) {
		element->Render3D();
	}

	//環境描画
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//ライトOFF
	feald->Draw();										//地面描画
	s_mesh->Draw(DEV, &s_mat);							//空描画(一時的)
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//ライトON

}


void GameMain::Render2D()
{
	//2D描画
	for (auto element : characterList) {
		element->Render2D();
	}

}

