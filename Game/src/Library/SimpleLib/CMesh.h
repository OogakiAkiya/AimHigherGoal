//==========================================================================================================
// 
// CMesh.h
//
// 制作者：鎌田
//
//==========================================================================================================

#ifndef CMesh_h
#define CMesh_h

namespace SimpleLib{

//=================================================================
// メッシュクラス
//	Xファイルモデルを読み込み・描画(固定機能)が可能。
//　解放は自動で行われる
//=================================================================
class CMesh{
public:
	//===============================================================================================
	// 情報取得系
	//===============================================================================================
	// メッシュ取得
	LPD3DXMESH					GetMesh(){return m_pMesh;}

	// マテリアル数取得
	int							GetMaterialCnt(){return (int)m_Materials.size();}

	// テクスチャ配列取得
	std::vector<CTexture*>&		GetTextures() { return m_Textures; }

	// 法線マップテクスチャ配列取得
	std::vector<CTexture*>&		GetNormalTextures() { return m_NormalTextures; }

	// マテリアルテーブル取得
	std::vector<D3DMATERIAL9>&	GetMaterials(){return m_Materials;}	

	// 隣接性データ取得
	LPD3DXBUFFER				GetAdjacency(){return m_pAdjacency;}

	//===============================================================================================
	// 読み込み
	//===============================================================================================
	// Xファイル読み込み
	bool LoadXFile(LPDIRECT3DDEVICE9 lpD3DDev,const char *lpFileName);

	//===============================================================================================
	// 解放
	//===============================================================================================
	// メッシュ解放
	void Release();

	//===============================================================================================
	// 描画関係
	//===============================================================================================
	// 固定機能でのメッシュ描画
	void Draw();

	// 
	CMesh() {}
	//
	~CMesh() {
		Release();
	}

private:

	//---------------
	// メッシュ
	//---------------
	LPD3DXMESH						m_pMesh = nullptr;
	
	//---------------
	// マテリアル関係
	//---------------
	std::vector<D3DMATERIAL9>		m_Materials;	// マテリアル配列

	std::vector<CTexture*>			m_Textures;			// テクスチャ配列　テクスチャ本体はm_TextureMapの方です。これはそっちへの参照(ポインタ)用
	std::vector<CTexture*>			m_NormalTextures;

	// 同名のテクスチャを重複して読み込んでメモリを無駄にするのを防ぐためのもの
	std::map<std::string, CTexture>	m_TextureMap;

	//---------------
	// その他
	//---------------
	LPD3DXBUFFER					m_pAdjacency = nullptr;		// 隣接性データ

	std::string						m_FileName;					// ファイル名

	LPDIRECT3DDEVICE9				m_lpD3DDev = nullptr;		// Direct3D Device記憶用

	CTexture* LoadTexture_UseMap(const char* filename);

private:
	// コピー禁止用
	CMesh(const CMesh& src){}
	void operator=(const CMesh& src){}
};

}
#endif
