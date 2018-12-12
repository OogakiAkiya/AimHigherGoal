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
	DEV->SetTransform(D3DTS_VIEW, SCENE.GetCameraView());
	DEV->SetTransform(D3DTS_PROJECTION, SCENE.GetCameraProj());

	// 3D描画
	Render3D();

	// 2D描画
	Render2D();

	DX.SceneEnd();
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev,
	LPSTR lpszCmdParam, int nCmdshow)
{
	VisualCWrapper::GetInstance();

	//シングルトン作成
	D3D::GetInstance();														//DirectX関係の処理が入っている

	if(WIN.Init(hInst,hPrev,lpszCmdParam,nCmdshow)==false)return 0;

	//DXRIVRATY系の初期化
	DX.Init(*WIN.GetHwnd());

	LPDIRECT3DDEVICE9 t;
	t = DX.GetDevice();
	timeBeginPeriod(1);

	
	ClientToScreen(*WIN.GetHwnd(), MOUSE.GetMousePt());								//クライアント座標に変換
	SetCursorPos(MOUSE.GetMousePt()->x, MOUSE.GetMousePt()->y);
	
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
	SceneControl::GetInstance();											//シーンを一括で管理している


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

	// ゲームに関する終了処理 ---------------------------
	SceneControl::DeleteInstance();
	D3D::DeleteInstance();
	timeEndPeriod(1);
	int returnValue=(int)WIN.GetMsg()->wParam;
	VisualCWrapper::DeleteInstance();
	return returnValue;

}

