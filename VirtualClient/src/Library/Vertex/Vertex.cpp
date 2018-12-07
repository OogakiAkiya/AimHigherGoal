#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"Vertex.h"

Vertex::Vertex()
{
}

Vertex::~Vertex()
{
	texture->Release();
}

void Vertex::InitVertex(float _size,float _x,float _y,float _z)
{
	//座標
	v[0].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	v[1].Pos = D3DXVECTOR3(0.0f, 0.0f, _size);
	v[2].Pos = D3DXVECTOR3(_size, 0.0f, _size);
	v[3].Pos = D3DXVECTOR3(_size, 0.0f, 0.0f);
	//色
	v[0].Color = D3DCOLOR_ARGB(255, 255, 255, 255);
	v[1].Color = D3DCOLOR_ARGB(255, 255, 255, 255);
	v[2].Color = D3DCOLOR_ARGB(255, 255, 255, 255);
	v[3].Color = D3DCOLOR_ARGB(255, 255, 255, 255);
	//テクスチャを張る位置
	v[0].Tex = D3DXVECTOR2(0.0f, 0.0f);
	v[1].Tex = D3DXVECTOR2(1.0f, 0.0f);
	v[2].Tex = D3DXVECTOR2(1.0f, 1.0f);
	v[3].Tex = D3DXVECTOR2(0.0f, 1.0f);
	D3DXMatrixIdentity(&mat);
	D3DXMatrixTranslation(&mat, _x, _y, _z);
}

void Vertex::RotationX(float _x)
{
	if (_x != NULL)D3DXMatrixRotationX(&mat, D3DXToRadian(_x));
}

void Vertex::RotationY(float _y)
{
	if (_y != NULL)D3DXMatrixRotationY(&mat, D3DXToRadian(_y));
}

void Vertex::RotationZ(float _z)
{
	if (_z != NULL)D3DXMatrixRotationZ(&mat, D3DXToRadian(_z));
}

void Vertex::LoadText(char _fname[], int _width, int _height, D3DCOLOR _color)
{
	width = (float)_width;
	height = (float)_height;
	//画像読み込み
	if (_width == 0)_width = D3DX_DEFAULT;
	if (_height == 0)_height = D3DX_DEFAULT;
	D3DXCreateTextureFromFileEx(DEV, _fname, _width, _height, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, _color, NULL, NULL, &texture);
}

void Vertex::Draw()
{
	//描画
	DEV->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);								//板ポリ用のフォーマットに戻す(板ポリを描画するときは使用する)
	DEV->SetTexture(0, texture);													//テクスチャのセット
	DEV->SetTransform(D3DTS_WORLD, &mat);											//座標の指定
	DEV->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(Vertex::vertex));			//描画

}
