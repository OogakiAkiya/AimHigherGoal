#include<WinSock2.h>
#include<Ws2tcpip.h>
#include"Include.h"
#include"Scene/SceneControl.h"
#include"Scene/SceneBase.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


// 更新処理
void Update(void)
{
	SCENE.Update();
}

// 3D描画
void Render3D(void)
{
	SCENE.Render3D();
}

// 2D描画
void Render2D(void)
{
	//描画開始
	DX.SpriteBegin();
	SCENE.Render2D();
	// 描画終了
	DX.SpriteEnd();
}

void GameFrame(void)
{
	DX.SceneBegin();

	// 更新処理
	Update();


	//行列設定
	DEV->SetTransform(D3DTS_VIEW, &SCENE.GetCameraView());
	DEV->SetTransform(D3DTS_PROJECTION, &SCENE.GetCameraProj());

	// 3D描画
	Render3D();

	// 2D描画
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

	//シングルトン作成
	D3D::GetInstance();														//DirectX関係の処理が入っている

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

	//DXRIVRATY系の初期化
	DX.Init(hwnd);

	LPDIRECT3DDEVICE9 t;
	t = DX.GetDevice();
	timeBeginPeriod(1);

	ClientToScreen(hwnd, &MOUSE.GetMousePt());								//クライアント座標に変換
	SetCursorPos(MOUSE.GetMousePt().x, MOUSE.GetMousePt().y);

	//フォグ
	DEV->SetRenderState(D3DRS_FOGENABLE, TRUE);								//フォグ有効化
	DEV->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(255, 255, 255));		//色の指定																		
																			//lpD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);		//頂点フォグ(頂点単位でフォグの設定)
	DEV->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);					//ピクセルフォグ(頂点単位でフォグの設定)

	float Start = 50.0f, End = 120.0f;
	DEV->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&Start));
	DEV->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&End));


	// ゲームに関する初期化処理 ---------------------------
	//クライアント処理
	Client::GetInstance();
	CLIENT.StartThread();

	SceneControl::GetInstance();											//シーンを一括で管理している


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

	// ゲームに関する終了処理 ---------------------------
	Client::DeleteInstance();
	SceneControl::DeleteInstance();
	D3D::DeleteInstance();

	timeEndPeriod(1);
	return (int)msg.wParam;

}

