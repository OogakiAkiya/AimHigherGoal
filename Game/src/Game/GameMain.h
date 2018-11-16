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
	Ground* feald;						//地面
	std::vector<CharacterBase*> characterlist;
	//お試し
	XFILE* s_mesh;						//メッシュ
	D3DXMATRIX s_mat;					//座標用行列
};

#endif