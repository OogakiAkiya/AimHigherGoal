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
	ipBuf.resize(256);
	idBuf.resize(256);

	//フォーム設定
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

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
}

void TitleMain::Render3D()
{
	//ライトOFF
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//ライトの陰影無し(この処理がないとskyがぼやけて見える)

	//ライトON
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//ライトの陰影無し(この処理がないとskyがぼやけて見える)

}

void TitleMain::Render2D()
{
	title->Draw(0, 0, 1280, 720);
	imgui->Draw();

}


void CleanString(std::string* _str,int* _strlen) {
	_str->clear();
	_str->resize(256);
	_strlen = 0;
}

void TitleMain::ImguiUpdate()
{
	imgui->UpdataStart();												//imgui開始

	ImGui::Begin("Form    ",NULL,window_flags);						//ウインドウ作成
	//ウインドウに配置するオブジェクト
	if (ImGui::InputText("IPAddress", (char*)ipBuf.c_str(), ipBuf.length()))ipBufLen++;		//テキストフォーム
	if (ImGui::Button("IP all erase"))CleanString(&ipBuf, &ipBufLen);                         //ボタン生成
	if (ImGui::InputText("ID", (char*)idBuf.c_str(), idBuf.length()))idBufLen++;			//テキストフォーム
	if (ImGui::Button("ID all erase"))CleanString(&idBuf, &idBufLen);							//ボタン生成
	ImGui::Text(msg.c_str());																//テキストフィールド

	//決定ボタン処理
	if (ImGui::Button("Decision")) {
		bool successFlg = true;
		//文字列の余分な空白を削除
		ipBuf.resize(ipBufLen);
		idBuf.resize(idBufLen);

		//ipアドレス処理
		if (ipBufLen == 0&&successFlg==true) {
			//ip未入力の場合(最終的にはなくなる予定)
			if (CLIENT.CreateSocket() == false) {						//サーバーとの接続処理
				//サーバーが見つからなかった
				msg = "Server not found";
				successFlg = false;
			}
		}
		else {
			if (CLIENT.CreateSocket(ipBuf) == false) {					//サーバーとの接続処理
				//サーバーが見つからなかった
				msg = "Server not found";
				successFlg = false;
			}
		}

		//プレイヤーID処理
		if (idBufLen == 0&&successFlg==true) {
			//ID未入力
			msg = "ID not input";
			successFlg = false;
		}

		//Game開始
		if (successFlg) {
			//入力に問題がなかった
			CLIENT.GetPlayerData()->SetId(idBuf);
			endflg = true;
		}
		if(endflg==true)ImGui::Text("Now Loading...");																//テキストフィールド

		//再入力可能な状態にする
		ipBuf.resize(256);
		idBuf.resize(256);

	}

	imgui->UpdataEnd();													//imgui終了

}

