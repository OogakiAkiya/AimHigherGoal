#ifndef SceneBase_h
#define SceneBase_h
class Camera;
class SceneBase
{
public:
	virtual ~SceneBase();
	virtual void Update();
	virtual void Render3D();
	virtual void Render2D();

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	Camera* GetCamera();
	bool GetEndFlg();

protected:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	Camera* camera;										//シーンのメインカメラをセット
	bool endflg=false;									//シーンチェンジ
};

#endif