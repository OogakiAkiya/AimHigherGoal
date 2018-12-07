class ImageSprite {
public:
	ImageSprite();
	~ImageSprite();
	void Load(std::string _pass, int _width, int _height);
	void Draw(int _startpointx, int _startpointy, int _endpointx, int _endpointy,int _alpha=255,bool _rotationflg=false);
private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void LoadText(LPDIRECT3DTEXTURE9 *lpTex, char fname[], int W, int H, D3DCOLOR Color);

	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	LPDIRECT3DTEXTURE9 texture;	//�e�N�X�`�����[�h�p
	D3DXMATRIX mat;				//���W
	int width=0;
	int height = 0;
};