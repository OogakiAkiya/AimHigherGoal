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
	//変数
	//---------------------------------------------------------
	std::unique_ptr<Ground> feald;						//地面
	std::vector<std::shared_ptr<CharacterBase>> characterList;
	//お試し
	std::weak_ptr<XFILE> s_mesh;						//メッシュ
	D3DXMATRIX s_mat;									//座標用行列
	std::weak_ptr<XFILE> boxMesh;						//メッシュ
	D3DXMATRIX boxMat;									//座標用行列

};

#endif