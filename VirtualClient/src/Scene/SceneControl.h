#ifndef SceneControl_h
#define SceneControl_h

class SceneBase;
class SceneControl
{
public:
	void Update();									//���X�V
	void Render3D();								//3D�`��
	void Render2D();								//2D�`��

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	D3DXMATRIX* GetCameraView();						//�J������View�s��擾
	D3DXMATRIX* GetCameraProj();
	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static SceneControl & GetInstance();			//�C���X�^���X�̍쐬
	static void DeleteInstance();					//�C���X�^���X�̍폜

private:
	SceneControl();
	~SceneControl();

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::shared_ptr<Camera> camera;									//���_�ɐݒ肷��p�J����
	std::shared_ptr<SceneBase> scene;								//���݂̃V�[�����i�[

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static SceneControl* s_Instance;				//�C���X�^���X
	SceneControl & operator=(SceneControl &);
	SceneControl(SceneControl&);
};
#define SCENE SceneControl::GetInstance()

#endif