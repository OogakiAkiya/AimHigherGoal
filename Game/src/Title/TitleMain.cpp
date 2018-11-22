#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"TitleMain.h"

TitleMain::TitleMain()
{
	//インスタンスの生成
	camera = new Camera();
	//CAMERA.SetTarget(player);
	title = new ImageSprite();

	//画像ロード処理
	title->Load("images/Title.png", 1280, 720);

}

TitleMain::~TitleMain()
{
}

void TitleMain::Update()
{
	//マウス座標更新処理
	MOUSE.FreeUpdate();

	//タイトルエンド処理
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		endflg = true;
	}
}

void TitleMain::Render3D()
{
	//ライトOFF
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//ライトの陰影無し(この処理がないとskyがぼやけて見える)
	//ライトOFF
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//ライトの陰影無し(この処理がないとskyがぼやけて見える)

}

void TitleMain::Render2D()
{
	title->Draw(0, 0, 1280, 720);
}

