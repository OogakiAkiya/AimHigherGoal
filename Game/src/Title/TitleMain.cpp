#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"TitleMain.h"

TitleMain::TitleMain()
{
	//�C���X�^���X�̐���
	camera = new Camera();
	//CAMERA.SetTarget(player);
	title = new ImageSprite();

	//�摜���[�h����
	title->Load("images/Title.png", 1280, 720);

}

TitleMain::~TitleMain()
{
}

void TitleMain::Update()
{
	//�}�E�X���W�X�V����
	MOUSE.FreeUpdate();

	//�^�C�g���G���h����
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		endflg = true;
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
}

