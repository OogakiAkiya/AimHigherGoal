#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"TitleMain.h"

TitleMain::TitleMain()
{
	camera = new Camera();
	//CAMERA.SetTarget(player);

	title = new ImageSprite();
	title->Load("images/Title.png", 1280, 720);

}

TitleMain::~TitleMain()
{
}

void TitleMain::Update()
{
	MOUSE.FreeUpdate();
	if (GetAsyncKeyState('W') & 0x8000) {
		endflg = true;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		char buf;
		float value = CLIENT.GetCount();
		buf = 0x12;
		send(CLIENT.GetSocket(), (char*)&buf, sizeof(buf), 0);			//文字の送信
		send(CLIENT.GetSocket(), (char*)&value, sizeof(value), 0);			//文字の送信
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

