#ifndef SceneControl_h
#define SceneControl_h

class SceneBase;
class SceneControl
{
public:
	void Update();									//���X�V
	void Render3D();								//3D�`��
	void Render2D();								//2D�`��
	D3DXMATRIX GetCameraView();						//�J������View�s��擾
	D3DXMATRIX GetCameraProj();
	// �V���O���g��
	static SceneControl & GetInstance();			//�C���X�^���X�̍쐬
	static void DeleteInstance();					//�C���X�^���X�̍폜

private:
	SceneControl();
	~SceneControl();

	Camera* camera;									//���_�ɐݒ肷��p�J����
	SceneBase* scene;								//���݂̃V�[�����i�[
	static SceneControl* s_Instance;				//�C���X�^���X
	SceneControl & operator=(SceneControl &);
	SceneControl(SceneControl&);
};
#define SCENE SceneControl::GetInstance()

#endif