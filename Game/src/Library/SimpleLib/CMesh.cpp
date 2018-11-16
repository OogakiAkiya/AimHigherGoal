#include "SimpleLib.h"

using namespace SimpleLib;


bool CMesh::LoadXFile(LPDIRECT3DDEVICE9 lpD3DDev,const char *lpFileName)
{
	Release();

	// デバイス記憶
	m_lpD3DDev = lpD3DDev;
	m_lpD3DDev->AddRef();	// 参照カウンタを１増やす

	// パス、ファイル名分解
	std::string Path;
	char szDir  [_MAX_PATH];	// フォルダ
	char szFname[_MAX_FNAME];	// ファイル名(拡張子を除く)
	char szExt  [_MAX_EXT];		// 拡張子
	_splitpath( lpFileName, nullptr, szDir, szFname, szExt);
	Path = szDir;

	m_FileName = szFname;
	m_FileName += szExt;

	HRESULT hr;

	LPD3DXBUFFER	pD3DXMtrlBuffer;

	// Xファイル読み込み
	DWORD mateNum;
	hr = D3DXLoadMeshFromX( lpFileName, 
							D3DXMESH_MANAGED, 
							lpD3DDev, 
							&m_pAdjacency, 
							&pD3DXMtrlBuffer, // マテリアルデータを格納するための変数を指定
							nullptr,
							&mateNum, // マテリアル数を格納するための変数を指定
							&m_pMesh);	// メッシュデータを格納するための変数を指定
	if(FAILED(hr)) return false;

	// メッシュのマテリアルの総数に基づいて、マテリアル配列を作成
	m_Textures.resize(mateNum);
	m_NormalTextures.resize(mateNum);
	m_Materials.resize(mateNum);

	// pD3DXMtrlBuffer から、質感やテクスチャーの情報を読み取る
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	// マテリアルコピー、テクスチャ読み込み
	std::string strTexFile;
	for(DWORD i=0; i<mateNum; i++ ){
		// 材質
		m_Materials[i] = d3dxMaterials[i].MatD3D;			// 質感のコピー
		m_Materials[i].Ambient = m_Materials[i].Diffuse;	// AmbientはDiffuseのコピーを使用

		// テクスチャ名 Path + ファイル名
		strTexFile = Path;
		if (d3dxMaterials[i].pTextureFilename) { strTexFile += d3dxMaterials[i].pTextureFilename; }

		// ※同じテクスチャを重複して読み込むのを防止するため、std::mapでテクスチャを管理しています

		// ファイル名をキーに、テクスチャ管理マップから検索・取得/読み込み
		m_Textures[i] = LoadTexture_UseMap(strTexFile.c_str());

		// 法線マップ
		std::string normalTexFile = ConvertExtFileName(strTexFile, "normal");
		m_NormalTextures[i] = LoadTexture_UseMap(normalTexFile.c_str());

//		ConvertExtFileName
	}

	// 作業用マテリアル バッファ開放
	SAFE_RELEASE(pD3DXMtrlBuffer);

	return true;
}
CTexture* CMesh::LoadTexture_UseMap(const char* filename)
{
	CTexture* tex = nullptr;
	// ファイル名をキーに、テクスチャ管理マップから検索・取得
	auto it = m_TextureMap.find(filename);
	if (it == m_TextureMap.end()) {	// 存在しない
									// 新規作成
		tex = &m_TextureMap[filename];
		// テクスチャ読み込み
		tex->LoadTexture(m_lpD3DDev, filename);
	}
	else {
		// 発見したやつをそのまま使用
		tex = &(*it).second;
	}
	return tex;
}


void CMesh::Release()
{

	m_TextureMap.clear();

	m_Textures.clear();
	m_NormalTextures.clear();
	m_Materials.clear();

	// 隣接面データ削除
	SAFE_RELEASE(m_pAdjacency);

	// メッシュ オブジェクト解放
	SAFE_RELEASE(m_pMesh);

	// デバイスの参照カウンタを減らす
	SAFE_RELEASE(m_lpD3DDev);

	m_FileName = "";
}

void CMesh::Draw()
{
	if (m_pMesh == nullptr)return;

	// マテリアルの数ぶんループ
	for(UINT i=0; i < m_Materials.size(); i++ ){
		// i番目のマテリアルを設定
		m_lpD3DDev->SetMaterial( &m_Materials[i] );
		// i番目のテクスチャを設定
		m_lpD3DDev->SetTexture( 0, m_Textures[i]->GetTex() );

		// i番目のマテリアルのメッシュを描画
		m_pMesh->DrawSubset( i );
	}
}
