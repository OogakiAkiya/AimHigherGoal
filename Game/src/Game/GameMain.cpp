#include"../Include.h"
#include"../Scene/SceneBase.h"
#include"Ground.h"
#include"../Game/Enemy/Enemy.h"
#include"Player/Player.h"
#include"GameMain.h"


GameMain::GameMain()
{
	//�}�b�v����
	feald = std::make_unique<Ground>();

	//�L��������
	std::shared_ptr<CharacterBase> temp = std::make_shared<Player>();
	characterList.push_back(temp);

	for (int i = 0;i<3; i++) {
		temp = std::make_shared<Enemy>(i);
		characterList.push_back(temp);
	}

	//�J�����ݒ�
	camera = characterList.at(0)->GetCamera();

	//�}�E�X�|�C���^��\��
	ShowCursor(FALSE);

	//��(�ꎞ�I�ɍ쐬)
	D3DXMatrixTranslation(&s_mat, 0.0f, 0.0f, 1.0f);
	s_mesh=CHECKMESH.Get("images/Sky.x");
}

GameMain::~GameMain()
{
	//�������
	feald=nullptr;								//�n�ʍ폜
	for (int i = 0; i < characterList.size(); i++) {
		characterList[i] = nullptr;
	}
	characterList.clear();
}

void GameMain::Update()
{
	//�}�E�X�|�C���^���W�X�V����
	MOUSE.ThirdPersonPointUpdata();

	//�L�����̍X�V����
	for (auto element : characterList) {
		element->Update();
	}

	//�N���C�A���g�ɑ����Ă��Ă���f�[�^�폜(�����ɂ���̂̓G�l�~�[�S�̂ɓ���𔽉f�����邽��)
	if (CLIENT.DataEmpty() == false) {
		CLIENT.Lock();
		CLIENT.DeleteData();											//�擾�f�[�^�폜
		CLIENT.Unlock();
	}

	//�J�����Z�b�g
	camera = characterList.at(0)->GetCamera();
}

void GameMain::Render3D()
{
	//�L�����`��
	for (auto element : characterList) {
		element->Render3D();
	}

	//���`��
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//���C�gOFF
	feald->Draw();										//�n�ʕ`��
	s_mesh.lock()->Draw(DEV, &s_mat);							//��`��(�ꎞ�I)
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//���C�gON

}


void GameMain::Render2D()
{
	//2D�`��
	for (auto element : characterList) {
		element->Render2D();
	}

}

