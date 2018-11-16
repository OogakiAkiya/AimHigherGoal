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
	DEV->SetTransform(D3DTS_VIEW, &SCENE.GetCameraView());
	DEV->SetTransform(D3DTS_PROJECTION, &SCENE.GetCameraProj());

	// 3D�`��
	Render3D();

	// 2D�`��
	Render2D();

	DX.SceneEnd();
}

LRESULT APIENTRY WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
	LPSTR lpszCmdParam, int nCmdshow)
{
	MSG msg;

	HWND hwnd;
	WNDCLASS wc;
	char szAppName[] = "Generic Game SDK Window";

	//�V���O���g���쐬
	D3D::GetInstance();														//DirectX�֌W�̏����������Ă���

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WndFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		szAppName,
		"Direct X",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		DX.SCRW, DX.SCRH,
		NULL, NULL, hInst,
		NULL);

	if (!hwnd)return FALSE;

	ShowWindow(hwnd, nCmdshow);
	UpdateWindow(hwnd);
	SetFocus(hwnd);

	//DXRIVRATY�n�̏�����
	DX.Init(hwnd);

	LPDIRECT3DDEVICE9 t;
	t = DX.GetDevice();
	timeBeginPeriod(1);

	ClientToScreen(hwnd, &MOUSE.GetMousePt());								//�N���C�A���g���W�ɕϊ�
	SetCursorPos(MOUSE.GetMousePt().x, MOUSE.GetMousePt().y);

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
	Client::GetInstance();
	CLIENT.StartThread();

	SceneControl::GetInstance();											//�V�[�����ꊇ�ŊǗ����Ă���


	//MainLoop---------------------------------------------
	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			GameFrame();
		}
	}

	// �Q�[���Ɋւ���I������ ---------------------------
	Client::DeleteInstance();
	SceneControl::DeleteInstance();
	D3D::DeleteInstance();

	timeEndPeriod(1);
	return (int)msg.wParam;

}

