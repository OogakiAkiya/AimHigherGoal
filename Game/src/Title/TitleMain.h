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
	//•Ï”
	//---------------------------------------------------------
	int states = 0;
	ImageSprite* title;						//ƒ^ƒCƒgƒ‹‰æ‘œ
};

#endif