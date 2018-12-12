#include<WinSock2.h>
#include<Ws2tcpip.h>
#include"Include.h"
#include"Scene/SceneControl.h"
#include"Scene/SceneBase.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


// �X�V����
void Update(void)
{
	SCENE.Update();
}

// 3D�`��
void Render3D(void)
{
	SCENE.Render3D();
}

// 2D�`��
void Render2D(void)
{
	//�`��J�n
	DX.SpriteBegin();
	SCENE.Render2D();
	// �`��I��
	DX.SpriteEnd();
}

void GameFrame(void)
{
	DX.SceneBegin();

	// �X�V����
	Update();


	//�s��ݒ�
	DEV->SetTransform(D3DTS_VIEW, SCENE.GetCameraView());
	DEV->SetTransform(D3DTS_PROJECTION, SCENE.GetCameraProj());

	// 3D�`��
	Render3D();

	// 2D�`��
	Render2D();

	DX.SceneEnd();
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
	LPSTR lpszCmdParam, int nCmdshow)
{
	VisualCWrapper::GetInstance();

	//�V���O���g���쐬
	D3D::GetInstance();														//DirectX�֌W�̏����������Ă���

	if(WIN.Init(hInst,hPrev,lpszCmdParam,nCmdshow)==false)return 0;

	//DXRIVRATY�n�̏�����
	DX.Init(*WIN.GetHwnd());

	LPDIRECT3DDEVICE9 t;
	t = DX.GetDevice();
	timeBeginPeriod(1);

	
	ClientToScreen(*WIN.GetHwnd(), MOUSE.GetMousePt());								//�N���C�A���g���W�ɕϊ�
	SetCursorPos(MOUSE.GetMousePt()->x, MOUSE.GetMousePt()->y);
	
	//�t�H�O
	DEV->SetRenderState(D3DRS_FOGENABLE, TRUE);								//�t�H�O�L����
	DEV->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(255, 255, 255));		//�F�̎w��																		
																			//lpD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);		//���_�t�H�O(���_�P�ʂŃt�H�O�̐ݒ�)
	DEV->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);					//�s�N�Z���t�H�O(���_�P�ʂŃt�H�O�̐ݒ�)

	float Start = 50.0f, End = 120.0f;
	DEV->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&Start));
	DEV->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&End));


	// �Q�[���Ɋւ��鏉�������� ---------------------------
	//�N���C�A���g����
	SceneControl::GetInstance();											//�V�[�����ꊇ�ŊǗ����Ă���


	//MainLoop---------------------------------------------
	while (1) {
		if (PeekMessage(WIN.GetMsg(), NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(WIN.GetMsg(), NULL, 0, 0))
				break;
			TranslateMessage(WIN.GetMsg());
			DispatchMessage(WIN.GetMsg());
		}
		else {
			GameFrame();
		}
	}

	// �Q�[���Ɋւ���I������ ---------------------------
	SceneControl::DeleteInstance();
	D3D::DeleteInstance();
	timeEndPeriod(1);
	int returnValue=(int)WIN.GetMsg()->wParam;
	VisualCWrapper::DeleteInstance();
	return returnValue;

}

