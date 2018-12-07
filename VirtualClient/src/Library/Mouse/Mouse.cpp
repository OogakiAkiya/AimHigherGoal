#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"Mouse.h"

Mouse* Mouse::s_Instance = nullptr;

Mouse::Mouse()
{
	//初期値設定
	mouseBasePt.x = 640;						//スクリーン座標の画面内の中央を初期位置とする
	mouseBasePt.y = 360;						//スクリーン座標の画面内の中央を初期位置とする

}

Mouse::~Mouse()
{
}

void Mouse::ThirdPersonPointUpdata()
{

	POINT mousePt;											//ポインタの位置を保存する
	long moveDis;											//誤差は例外とみなす処理

	//マウスポインタの移動距離の取得
	GetCursorPos(&mousePt);									//ポインタの位置の取得
	moveDis = (mousePt.x - mouseBasePt.x);
	if (moveDis > 5 || moveDis < -5) {
		movey += (mousePt.x - mouseBasePt.x) / SENSITIVITY;			//左右の回転
	}

	moveDis = (mousePt.y - mouseBasePt.y);
	if (abs(moveDis) > 5) {
		movex += (mousePt.y - mouseBasePt.y) / SENSITIVITY;			//後ろの2.0fは感度(大きいほど鈍くなる)
	}
	if (movex > 80)movex = 80;
	if (movex < -80)movex = -80;
	SetCursorPos(mouseBasePt.x, mouseBasePt.y);
}

void Mouse::FreeUpdate()
{
	GetCursorPos(&mouseBasePt);									//ポインタの位置の取得
}


POINT Mouse::GetMousePt()
{
	return mouseBasePt;
}

float Mouse::GetMoveX()
{
	return movex;
}

float Mouse::GetMoveY()
{
	return movey;
}

Mouse & Mouse::GetInstance()
{

	if (s_Instance == nullptr) { s_Instance = new Mouse(); }
	return *s_Instance;

}

void Mouse::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}
}
