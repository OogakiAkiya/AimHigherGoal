#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"ImageSprite.h"


ImageSprite::ImageSprite()
{
	D3DXMatrixIdentity(&mat);
}

ImageSprite::~ImageSprite()
{
}

void ImageSprite::Load(std::string _pass,int _width,int _height)
{
	width = _width;
	height = _height;

	LoadText(&texture,(char*)_pass.c_str(),width,height, NULL);
}

void ImageSprite::Draw(int _startpointx, int _startpointy, int _endpointx, int _endpointy,int _alpha,bool _rotationflg)
{
	RECT rc = { _startpointx,_startpointy,_endpointx,_endpointy };
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&temp);
	DXSP->SetTransform(&temp);
	if (_rotationflg == false) {
		DXSP->Draw(texture/*テクスチャ*/, &rc/*取り込み範囲*/, &D3DXVECTOR3(0.0f, 0.0f, 0.0f)/*中心ベクトル*/, NULL/*NULL以外書かない*/, D3DCOLOR_ARGB(_alpha, 255, 255, 255));
	}
	else {
		DXSP->Draw(texture/*テクスチャ*/, &rc/*取り込み範囲*/, &D3DXVECTOR3(width/2, height/2, 0.0f)/*中心ベクトル*/, NULL/*NULL以外書かない*/, D3DCOLOR_ARGB(_alpha, 255, 255, 255));
	}
}

void ImageSprite::LoadText(LPDIRECT3DTEXTURE9 * lpTex, char fname[], int W, int H, D3DCOLOR Color)
{
	if (W == 0)W = D3DX_DEFAULT;
	if (H == 0)H = D3DX_DEFAULT;
	D3DXCreateTextureFromFileEx(DEV, fname, W, H, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, Color, NULL, NULL, lpTex);
}