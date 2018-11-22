class ImageSprite {
public:
	ImageSprite();
	~ImageSprite();
	void Load(std::string _pass, int _width, int _height);
	void Draw(int _startpointx, int _startpointy, int _endpointx, int _endpointy,int _alpha=255,bool _rotationflg=false);
private:
	//---------------------------------------------------------
	//ローカル関数
	//---------------------------------------------------------
	void LoadText(LPDIRECT3DTEXTURE9 *lpTex, char fname[], int W, int H, D3DCOLOR Color);

	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	LPDIRECT3DTEXTURE9 texture;	//テクスチャロード用
	D3DXMATRIX mat;				//座標
	int width=0;
	int height = 0;
};