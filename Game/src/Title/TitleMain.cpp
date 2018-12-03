#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"TitleMain.h"

TitleMain::TitleMain()
{
	//�C���X�^���X�̐���
	imgui = new ImguiWrapper(*WIN.GetHwnd());
	camera = new Camera();
	title = new ImageSprite();

	//�摜���[�h����
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
	//�}�E�X���W�X�V����
	MOUSE.FreeUpdate();

	//���̓t�H�[���X�V����
	ImguiUpdate();

	//�^�C�g���G���h����
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
	}
}

void TitleMain::Render3D()
{
	//���C�gOFF
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//���C�g�̉A�e����(���̏������Ȃ���sky���ڂ₯�Č�����)
	//���C�gOFF
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//���C�g�̉A�e����(���̏������Ȃ���sky���ڂ₯�Č�����)

}

void TitleMain::Render2D()
{
	title->Draw(0, 0, 1280, 720);
	imgui->Draw();

}

void TitleMain::ImguiUpdate()
{
	imgui->UpdataStart();

	ImGui::Begin("         ", imgui->GetApperFlg());						//�E�C���h�E�쐬

																			//�E�C���h�E�ɐݒ肷��I�u�W�F�N�g
	if (ImGui::InputText("ID", (char*)buf.c_str(), buf.length())){
		bufLength++;
	};

	if (ImGui::Button("All delete section")) {                           //�{�^������
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

