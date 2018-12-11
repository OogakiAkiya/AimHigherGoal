#include"../Include.h"
#include"SceneBase.h"
#include"../Game/Ground.h"
#include"../Game/Enemy/Enemy.h"
#include"../Game/Player/Player.h"
#include"../Game/GameMain.h"
#include"../Title/TitleMain.h"
#include"SceneControl.h"

SceneControl* SceneControl::s_Instance = nullptr;

SceneControl::SceneControl()
{
	//インスタンスの生成
	CheckMesh::GetInstance();												//メッシュ情報を重複して読み込まないためのクラス
	Mouse::GetInstance();													//マウスを管理する
	scene = std::make_unique<TitleMain>();
	//シーンメインカメラ設定
	camera = scene->GetCamera();						//視点用カメラの取得
}

SceneControl::~SceneControl()
{
	//解放処理
	scene = nullptr;
	Mouse::DeleteInstance();
	CheckMesh::DeleteInstance();

}

void SceneControl::Update()
{
	//シーンチェンジ処理
	if (scene->GetEndFlg() == true) {					//endflgがtrueになっていたらシーンチェンジ
		scene = std::make_unique<GameMain>();
	}
	//シーン更新処理
	scene->Update();									//保持するシーンの情報更新

	//カメラ情報取得
	camera = scene->GetCamera();						//視点用カメラの取得

}

void SceneControl::Render3D()
{
	scene->Render3D();
}

void SceneControl::Render2D()
{
	scene->Render2D();
}

D3DXMATRIX SceneControl::GetCameraView()
{
	return camera->GetView();
}

D3DXMATRIX SceneControl::GetCameraProj()
{
	return camera->GetProj();
}

SceneControl & SceneControl::GetInstance()
{
		if (s_Instance == nullptr) { s_Instance = new SceneControl(); }
		return *s_Instance;
}

void SceneControl::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}
}
