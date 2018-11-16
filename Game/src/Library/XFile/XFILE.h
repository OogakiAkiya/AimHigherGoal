#ifndef XFILE_h		//ModelShader_hは各ヘッダーごとに変えないとエラーが出る
#define XFILE_h


class XFILE {
public:
	XFILE();
	~XFILE();
	
	void Load(LPDIRECT3DDEVICE9 _lpD3DDevice,std::string _pass);
	void Draw(LPDIRECT3DDEVICE9 _lpD3DDevice,D3DXMATRIX *_World);
	void Release();
private:
	LPD3DXMESH lpMesh = NULL;			//メッシュの形状
	DWORD NumMaterial = 0;			//マテリアルの数
	D3DMATERIAL9 *Mat = NULL;			//マテリアルの内容
	LPDIRECT3DTEXTURE9 *Tex = NULL;	//テクスチャの内容

};
#endif