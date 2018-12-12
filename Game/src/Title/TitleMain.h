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
	//�t�H�[���Ɏg�p����ϐ�
	std::string ipBuf;
	int ipBufLen=0;
	std::string idBuf;
	int idBufLen=0;
	std::string msg="";
	ImGuiWindowFlags window_flags = 0;
	std::unique_ptr<ImguiWrapper> imgui;

	int states = 0;
	std::unique_ptr<ImageSprite> title;						//�^�C�g���摜
};

#endif