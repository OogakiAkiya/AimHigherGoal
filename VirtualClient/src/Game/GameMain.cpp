#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"../Library/Client/Client.h"
#include"GameMain.h"


GameMain::GameMain()
{
	imgui =std::make_shared<ImguiWrapper>(*WIN.GetHwnd());
	camera = std::make_shared<Camera>();
	data = std::make_shared<Data>();
}

GameMain::~GameMain()
{
	for (auto& client:clientList) {
		client = nullptr;
	}
	clientList.clear();
	camera=nullptr;
	imgui=nullptr;
}


void GameMain::Update()
{
	imgui->UpdataStart();												//imgui開始
	std::stringstream query;

	ImGui::Begin("Client Controler", imgui->GetApperFlg());						//ウインドウ作成
	//ウインドウに配置するオブジェクト

	const char* items[] = {"1","10", "100", "1000", "10000"};
	static int item_current = 0;
	ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));

	//追加ボタン
	if (ImGui::Button("ADD")) {
		int loop=0;
		if (item_current == 0)loop = 1;
		if (item_current == 1)loop = 10;
		if (item_current == 2)loop = 100;
		if (item_current == 3)loop = 1000;
		if (item_current == 4)loop = 5000;

		int nowSize = clientList.size();
		clientList.resize(nowSize + loop);
		for (int i = nowSize; i < loop+nowSize; i++) {
			std::shared_ptr<Client> temp = std::make_shared<Client>();
			temp->CreateSocket();
			query << i;
			temp->GetPlayerData()->SetId(query.str());
			query.str("");
			query.clear(std::stringstream::goodbit);

			//追加処理
			//clientList.push_back(temp);
			clientList[i] = temp;
			if (item_current == 4)Sleep(10);
		}
	}

	//接続数
	ImGui::Text("Client =%d", clientList.size());

	//クライアント削除処理
	if (ImGui::Button("DELETE")) {
		for (auto& client : clientList) {
			client = nullptr;
		}
		clientList.clear();
	}
	imgui->UpdataEnd();													//imgui終了


	//クライアント処理
	for (auto& client : clientList) {
		client->SendPos(client->GetPlayerData());
	}
}

void GameMain::Render3D()
{
	//環境描画
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//ライトOFF
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//ライトON

}


void GameMain::Render2D()
{
	imgui->Draw();
}

