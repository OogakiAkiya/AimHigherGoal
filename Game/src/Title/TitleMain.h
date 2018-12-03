#ifndef TitleMain_h
#define TitleMain_h

class TitleMain:public SceneBase
{
public:
	TitleMain();
	~TitleMain()override;
	void Update()override;
	void Render3D()override;
	void Render2D()override;
private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void ImguiUpdate();

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	std::string buf;
	int bufLength=0;
	ImguiWrapper* imgui;
	int states = 0;
	ImageSprite* title;						//�^�C�g���摜
};

#endif