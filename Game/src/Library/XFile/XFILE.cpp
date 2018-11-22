#include"../StandardLibraryInclude.h"
#include"XFILE.h"

XFILE::XFILE() {
}

XFILE::~XFILE() {
	Release(); 
}


void XFILE::Load(LPDIRECT3DDEVICE9 _lpD3DDevice, std::string _pass)
{
	LPD3DXBUFFER lpD3DXBuffer;

	D3DXLoadMeshFromX(_pass.c_str(), D3DXMESH_MANAGED, _lpD3DDevice, NULL, &lpD3DXBuffer, NULL, &(numMaterial), &(lpMesh));

	mat = new D3DMATERIAL9[numMaterial];
	texture = new LPDIRECT3DTEXTURE9[numMaterial];

	D3DXMATERIAL* D3DXMat = (D3DXMATERIAL*)lpD3DXBuffer->GetBufferPointer();

	DWORD i;
	for (i = 0; i<numMaterial; i++) {
		mat[i] = D3DXMat[i].MatD3D;
		mat[i].Ambient = mat[i].Diffuse;
		if (FAILED(D3DXCreateTextureFromFile(_lpD3DDevice, D3DXMat[i].pTextureFilename, &(texture[i])))) {
			texture[i] = NULL;
		}
	}

	lpD3DXBuffer->Release();
}


void XFILE::Draw(LPDIRECT3DDEVICE9 _lpD3DDevice,D3DXMATRIX* _World) {
	_lpD3DDevice->SetTransform(D3DTS_WORLD, _World);
	for (DWORD i = 0; i < numMaterial; i++)
	{
		_lpD3DDevice->SetMaterial(&(mat[i]));		//マテリアルの設定
		_lpD3DDevice->SetTexture(0, texture[i]);		//テクスチャの設定(何もなければNULLが入り何も起きない)
		lpMesh->DrawSubset(i);					//メッシュのi番目のグループを表示
	}
}



void XFILE::Release()
{
	if (lpMesh != NULL) {
		DWORD i;

		delete[] mat;
		for (i = 0; i<numMaterial; i++) {
			if (texture[i] != NULL) {
				texture[i]->Release();
				texture[i] = NULL;
			}
		}
		delete[] texture;
		lpMesh->Release();

		lpMesh = NULL;
	}
}
