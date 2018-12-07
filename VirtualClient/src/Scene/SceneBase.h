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
	//���擾
	//---------------------------------------------------------
	Camera* GetCamera();
	bool GetEndFlg();

protected:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	Camera* camera;										//�V�[���̃��C���J�������Z�b�g
	bool endflg=false;									//�V�[���`�F���W
};

#endif