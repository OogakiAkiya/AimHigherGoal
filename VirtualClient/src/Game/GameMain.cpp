#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"../Library/Client/Client.h"
#include"GameMain.h"


GameMain::GameMain()
{
	imgui = new ImguiWrapper(*WIN.GetHwnd());
	camera = new Camera();
	data = new Data();
}

GameMain::~GameMain()
{
	for (auto& client:clientList) {
		delete client;
	}
	clientList.clear();
	delete camera;
	delete imgui;
}


void GameMain::Update()
{
	imgui->UpdataStart();												//imgui�J�n
	std::stringstream query;

	ImGui::Begin("Client Controler", imgui->GetApperFlg());						//�E�C���h�E�쐬
	//�E�C���h�E�ɔz�u����I�u�W�F�N�g

	const char* items[] = {"1","10", "100", "1000", "10000"};
	static int item_current = 0;
	ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));

	//�ǉ��{�^��
	if (ImGui::Button("ADD")) {
		int loop=0;
		if (item_current == 0)loop = 1;
		if (item_current == 1)loop = 10;
		if (item_current == 2)loop = 100;
		if (item_current == 3)loop = 1000;
		if (item_current == 4)loop = 10000;

		for (int i = 0; i < loop; i++) {
			Client* temp = new Client();
			temp->CreateSocket();
			query << clientList.size();
			temp->GetPlayerData()->SetId(query.str());
			query.str("");
			query.clear(std::stringstream::goodbit);

			//�ǉ�����
			clientList.push_back(temp);
		}
	}

	//�ڑ���
	ImGui::Text("Client =%d", clientList.size());

	//�N���C�A���g�폜����
	if (ImGui::Button("DELETE")) {
		for (auto& client : clientList) {
			delete client;
		}
		clientList.clear();
	}
	imgui->UpdataEnd();													//imgui�I��


	//�N���C�A���g����
	for (auto& client : clientList) {
		client->SendPos(client->GetPlayerData());
	}
}

void GameMain::Render3D()
{
	//���`��
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//���C�gOFF
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//���C�gON

}


void GameMain::Render2D()
{
	imgui->Draw();
}

