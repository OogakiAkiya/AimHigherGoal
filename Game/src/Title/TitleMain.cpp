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
	ipBuf.resize(256);
	idBuf.resize(256);

	//�t�H�[���ݒ�
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

	//�}�E�X���W�X�V����
	MOUSE.FreeUpdate();

	//���̓t�H�[���X�V����
	ImguiUpdate();
}

void TitleMain::Render3D()
{
	//���C�gOFF
	DEV->SetRenderState(D3DRS_LIGHTING, FALSE);			//���C�g�̉A�e����(���̏������Ȃ���sky���ڂ₯�Č�����)

	//���C�gON
	DEV->SetRenderState(D3DRS_LIGHTING, TRUE);			//���C�g�̉A�e����(���̏������Ȃ���sky���ڂ₯�Č�����)

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
	imgui->UpdataStart();												//imgui�J�n

	ImGui::Begin("Form    ",NULL,window_flags);						//�E�C���h�E�쐬
	//�E�C���h�E�ɔz�u����I�u�W�F�N�g
	if (ImGui::InputText("IPAddress", (char*)ipBuf.c_str(), ipBuf.length()))ipBufLen++;		//�e�L�X�g�t�H�[��
	if (ImGui::Button("IP all erase"))CleanString(&ipBuf, &ipBufLen);                         //�{�^������
	if (ImGui::InputText("ID", (char*)idBuf.c_str(), idBuf.length()))idBufLen++;			//�e�L�X�g�t�H�[��
	if (ImGui::Button("ID all erase"))CleanString(&idBuf, &idBufLen);							//�{�^������
	ImGui::Text(msg.c_str());																//�e�L�X�g�t�B�[���h

	//����{�^������
	if (ImGui::Button("Decision")) {
		bool successFlg = true;
		//������̗]���ȋ󔒂��폜
		ipBuf.resize(ipBufLen);
		idBuf.resize(idBufLen);

		//ip�A�h���X����
		if (ipBufLen == 0&&successFlg==true) {
			//ip�����͂̏ꍇ(�ŏI�I�ɂ͂Ȃ��Ȃ�\��)
			if (CLIENT.CreateSocket() == false) {						//�T�[�o�[�Ƃ̐ڑ�����
				//�T�[�o�[��������Ȃ�����
				msg = "Server not found";
				successFlg = false;
			}
		}
		else {
			if (CLIENT.CreateSocket(ipBuf) == false) {					//�T�[�o�[�Ƃ̐ڑ�����
				//�T�[�o�[��������Ȃ�����
				msg = "Server not found";
				successFlg = false;
			}
		}

		//�v���C���[ID����
		if (idBufLen == 0&&successFlg==true) {
			//ID������
			msg = "ID not input";
			successFlg = false;
		}

		//Game�J�n
		if (successFlg) {
			//���͂ɖ�肪�Ȃ�����
			CLIENT.GetPlayerData()->SetId(idBuf);
			endflg = true;
		}
		if(endflg==true)ImGui::Text("Now Loading...");																//�e�L�X�g�t�B�[���h

		//�ē��͉\�ȏ�Ԃɂ���
		ipBuf.resize(256);
		idBuf.resize(256);

	}

	imgui->UpdataEnd();													//imgui�I��

}

