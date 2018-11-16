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
	Camera* GetCamera();
	bool GetEndFlg();
protected:
	Camera* camera;
	bool endflg=false;
};

#endif