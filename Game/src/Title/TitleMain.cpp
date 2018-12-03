#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"TitleMain.h"

TitleMain::TitleMain()
{
	//インスタンスの生成
	imgui = new ImguiWrapper(*WIN.GetHwnd());
	camera = new Camera();
	title = new ImageSprite();

	//画像ロード処理
	title->Load("images/Title.png", 1280, 720);
	buf.resize(256);

}

TitleMain::~TitleMain()
{
	delete imgui;
	delete camera;
	delete title;
}

void TitleMain::Update()
{
	//マウス座標更新処理
	MOUSE.FreeUpdate();

	//入力フォーム更新処理
	ImguiUpdate();

	//タイトルエンド処理
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
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
	imgui->Draw();

}

void TitleMain::ImguiUpdate()
{
	imgui->UpdataStart();

	ImGui::Begin("         ", imgui->GetApperFlg());						//ウインドウ作成

																			//ウインドウに設定するオブジェクト
	if (ImGui::InputText("ID", (char*)buf.c_str(), buf.length())){
		bufLength++;
	};

	if (ImGui::Button("All delete section")) {                           //ボタン生成
		buf.clear();
		bufLength = 0;
	}
	if (ImGui::Button("Decision")) {
		endflg = true;
		buf.resize(bufLength);
		CLIENT.GetPlayerData()->SetId(buf);
	}
	ImGui::Text("test");

	imgui->UpdataEnd();

}

