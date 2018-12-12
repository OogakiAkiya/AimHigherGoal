#ifndef SceneControl_h
#define SceneControl_h

class SceneBase;
class SceneControl
{
public:
	void Update();									//情報更新
	void Render3D();								//3D描画
	void Render2D();								//2D描画

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	D3DXMATRIX* GetCameraView();						//カメラのView行列取得
	D3DXMATRIX* GetCameraProj();
	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static SceneControl & GetInstance();			//インスタンスの作成
	static void DeleteInstance();					//インスタンスの削除

private:
	SceneControl();
	~SceneControl();

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::shared_ptr<Camera> camera;									//視点に設定する用カメラ
	std::shared_ptr<SceneBase> scene;								//現在のシーンを格納

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static SceneControl* s_Instance;				//インスタンス
	SceneControl & operator=(SceneControl &);
	SceneControl(SceneControl&);
};
#define SCENE SceneControl::GetInstance()

#endif