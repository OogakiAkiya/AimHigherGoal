#include"../StandardLibraryInclude.h"
#include"XFILE.h"

XFILE::XFILE() {}
XFILE::~XFILE() { Release(); }


void XFILE::Load(LPDIRECT3DDEVICE9 _lpD3DDevice, std::string _pass)
{
	LPD3DXBUFFER lpD3DXBuffer;

	D3DXLoadMeshFromX(_pass.c_str(), D3DXMESH_MANAGED, _lpD3DDevice, NULL, &lpD3DXBuffer, NULL, &(NumMaterial), &(lpMesh));

	Mat = new D3DMATERIAL9[NumMaterial];
	Tex = new LPDIRECT3DTEXTURE9[NumMaterial];

	D3DXMATERIAL* D3DXMat = (D3DXMATERIAL*)lpD3DXBuffer->GetBufferPointer();

	DWORD i;
	for (i = 0; i<NumMaterial; i++) {
		Mat[i] = D3DXMat[i].MatD3D;
		Mat[i].Ambient = Mat[i].Diffuse;
		if (FAILED(D3DXCreateTextureFromFile(_lpD3DDevice, D3DXMat[i].pTextureFilename, &(Tex[i])))) {
			Tex[i] = NULL;
		}
	}

	lpD3DXBuffer->Release();
}


void XFILE::Draw(LPDIRECT3DDEVICE9 _lpD3DDevice,D3DXMATRIX* _World) {
	_lpD3DDevice->SetTransform(D3DTS_WORLD, _World);
	for (DWORD i = 0; i < NumMaterial; i++)
	{
		_lpD3DDevice->SetMaterial(&(Mat[i]));		//マテリアルの設定
		_lpD3DDevice->SetTexture(0, Tex[i]);		//テクスチャの設定(何もなければNULLが入り何も起きない)
		lpMesh->DrawSubset(i);					//メッシュのi番目のグループを表示
	}
}



void XFILE::Release()
{
	if (lpMesh != NULL) {
		DWORD i;

		delete[] Mat;
		for (i = 0; i<NumMaterial; i++) {
			if (Tex[i] != NULL) {
				Tex[i]->Release();
				Tex[i] = NULL;
			}
		}
		delete[] Tex;
		lpMesh->Release();

		lpMesh = NULL;
	}
}
