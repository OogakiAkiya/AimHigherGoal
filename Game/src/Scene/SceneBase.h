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
	std::shared_ptr<Camera> GetCamera();
	bool GetEndFlg();

protected:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::shared_ptr<Camera> camera;										//シーンのメインカメラをセット
	bool endflg=false;									//シーンチェンジ
};

#endif