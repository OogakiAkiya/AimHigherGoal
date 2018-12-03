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
	//ローカル関数
	//---------------------------------------------------------
	void ImguiUpdate();

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	std::string buf;
	int bufLength=0;
	ImguiWrapper* imgui;
	int states = 0;
	ImageSprite* title;						//タイトル画像
};

#endif