#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"Ground.h"
#include"../Game/Enemy/Enemy.h"
#include"Player/Player.h"
#include"GameMain.h"


GameMain::GameMain()
{
	feald = new Ground();						//�n�ʍ쐬
	CharacterBase* temp = new Player();
	characterlist.push_back(temp);
	for (int i = 0;i<3; i++) {
		temp = new Enemy(i);
		characterlist.push_back(temp);
	}

	//camera = player->GetCamera();				//Player�̃J�������擾����
	camera = characterlist.at(0)->GetCamera();
	ShowCursor(FALSE);

	//��(�ꎞ�I�ɍ쐬)
	D3DXMatrixTranslation(&s_mat, 0.0f, 0.0f, 1.0f);
	s_mesh=CHECKMESH.Get("images/Sky.x");
}

GameMain::~GameMain()
{
	delete feald;								//�n�ʍ폜
	CharacterBase* deletechara;					//�L�����폜
	for (auto element : characterlist) {
		deletechara = element;
		delete deletechara;
	}
	characterlist.erase(characterlist.begin(), characterlist.end());
}

void GameMain::Update()
{
	MOUSE.ThirdPersonPointUpdata();							//�}�E�X�̍��W�X�V
	for (auto element : characterlist) {
		element->Update();
	}
	//�N���C�A���g�ɑ����Ă��Ă���f�[�^�폜(�����ɂ���̂̓G�l�~�[�S�̂ɓ���𔽉f�����邽��)
	if (CLIENT.DataEmpty() == false) {
		CLIENT.Lock();
		CLIENT.DeleteData();											//�擾�f�[�^�폜
		CLIENT.Unlock();
	}
	camera = characterlist.at(0)->GetCamera();
}

void GameMain::Render3D()
{
	for (auto element : characterlist) {
		element->Render3D();
	}

	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//���C�gOFF
	feald->Draw();										//�n�ʕ`��
	s_mesh->Draw(DEV, &s_mat);							//��`��(�ꎞ�I)
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//���C�gON

}


void GameMain::Render2D()
{
	//2D�`��
	for (auto element : characterlist) {
		element->Render2D();
	}

}

