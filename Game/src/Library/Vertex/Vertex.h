#ifndef Vertex_h
#define Vertex_h

class Vertex
{
public:
	Vertex();
	~Vertex();
	void InitVertex(float _size,float _x, float _y, float _z);						//初期化(_sizeは面のサイズ_x_y_zは面の描画する位置)
	void LoadText(char _fname[], int _width, int _height, D3DCOLOR _color);			//描画する画像ロード(_fnameはテクスチャのパス,_width幅,_hight高さ,_color透過する色NULLの指定可能)
	void Draw();																	//描画

private:
	//---------------------------------------------------------
	// 構造体
	//---------------------------------------------------------
	struct vertex{
		D3DXVECTOR3 Pos;						//座標
		D3DCOLOR Color;							//色
		D3DXVECTOR2 Tex;						//テクスチャの位置
	};

	//---------------------------------------------------------
	// 変数
	//---------------------------------------------------------
	vertex v[4];								//頂点
	D3DXMATRIX mat;								//座標用行列
	LPDIRECT3DTEXTURE9 texture;					//テクスチャ

};
#endif