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
	//変数
	//---------------------------------------------------------
	int states = 0;
	ImageSprite* title;						//タイトル画像
};

#endif