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
	//�ϐ�
	//---------------------------------------------------------
	std::unique_ptr<Ground> feald;						//�n��
	std::vector<std::shared_ptr<CharacterBase>> characterList;
	//������
	std::weak_ptr<XFILE> s_mesh;						//���b�V��
	D3DXMATRIX s_mat;					//���W�p�s��
};

#endif