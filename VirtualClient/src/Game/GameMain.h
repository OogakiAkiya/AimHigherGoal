#ifndef GameMain_h
#define GameMain_h

class GameMain:public SceneBase
{
public:
	GameMain();
	~GameMain()override;
	void Update()override;
	void Render3D()override;
	void Render2D()override;

private:
	//---------------------------------------------------------
	//•Ï”
	//---------------------------------------------------------
	ImguiWrapper* imgui;
	std::vector<Client*> clientList;
	Data* data;
};

#endif