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
	std::string ipBuf;
	int ipBufLen=0;
	std::string idBuf;
	int idBufLen=0;
	std::string msg="";

	ImguiWrapper* imgui;
	int states = 0;
	ImageSprite* title;						//タイトル画像
};

#endif