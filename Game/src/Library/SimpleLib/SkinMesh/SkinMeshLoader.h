#ifndef __SkinMeshLoader_h__
#define __SkinMeshLoader_h__

namespace SimpleLib
{

class CSkinMesh;
struct D3DXMESHCONTAINER_EX;

//--------------------------------------------------------------------------------------
// フレーム情報の追加版。階層構造で構成するので兄弟とか子のアドレスも持ってる。
//  ポーンのジョイント名も持ってますよ。
//  メンバ変数であるpMeshContainerになにか入っているやつは描画するのだ。
//--------------------------------------------------------------------------------------
struct D3DXFRAME_EX: public D3DXFRAME
{
	D3DXMATRIX			m_OffsetMat;					// オフセット行列
	DWORD				m_OffsetID;						// 行列テーブルのインデックス番号
	D3DXMATRIX			m_DefLocalMat;					// デフォルト姿勢でのローカル行列　特別なことがない限り使用しない
	D3DXFRAME_EX*		m_pMother;						// 親ボーンへのアドレス

	D3DXFRAME_EX() : m_pMother(0), m_OffsetID(-1){
	}

	D3DXMESHCONTAINER_EX* GetMeshContainer(){
		return (D3DXMESHCONTAINER_EX*)pMeshContainer;
	}

	// その他
	// Nameにフレーム名
	// TransformationMatrixに変換行列
	// pMeshContainerにメッシュコンテナへのアドレス。
	// pFrameSiblingに次の兄弟フレームのアドレス
	// pFrameFirstChildに自分の1番目の子供フレームへのアドレス
};

//--------------------------------------------------------------------------------------
//各部のメッシュ情報(形状、マテリアル、テクスチャなど)を保持
// つまりこいつを持っているフレームを描画することになる。
//--------------------------------------------------------------------------------------
struct D3DXMESHCONTAINER_EX: public D3DXMESHCONTAINER
{
	// テクスチャ配列
	std::vector<CTexture*>		m_Textures;			// テクスチャ配列　テクスチャ本体はm_TextureMapの方です。これはそっちへの参照(ポインタ)用
	std::vector<CTexture*>		m_NormalTextures;

	// スキンメッシュ情報
	LPD3DXMESH					m_pOrigMesh;				// 未加工メッシュ

	DWORD						m_dwWeight;					// 重みの個数
	DWORD						m_dwBoneCombiCnt;			// ボーンコンビネーションの数
	LPD3DXBUFFER				m_pBoneCombinationBuf;

	std::vector<D3DXFRAME_EX*>	m_BoneFrameArray;			// 影響するフレームへの参照配列。描画時にこのフレームの行列を使う。
	DWORD						m_NumPaletteEntries;

	// ソフトウェアスキニング用
	struct SoftwareSkiningData{
		LPD3DXMESH					pSoftSkinMesh;			// ソフトウェアスキニング適用用メッシュ
		LPD3DXATTRIBUTERANGE		pAttributeTable;
		DWORD						MaterialCnt;			// ボーンコンビネーションの数

		SoftwareSkiningData() : pSoftSkinMesh(NULL),
								pAttributeTable(NULL),
								MaterialCnt(0)
		{}
	};
	SoftwareSkiningData		m_SoftSkinData;				// ソフトウェアスキニング用データ

	D3DXMESHCONTAINER_EX() :
		m_pOrigMesh(NULL),
		m_dwBoneCombiCnt(0),
		m_dwWeight(0),
		m_pBoneCombinationBuf(NULL),
		m_NumPaletteEntries(0)
	{
	}
	// その他
	// Nameにメッシュ名
	// pMaterialsにマテリアルの配列
	// NumMaterialsにマテリアル数
	// pSkinInfoにスキンメッシュの情報
	// pNextMeshContainerに次のメッシュのアドレス。1フレームに複数のメッシュを持てる
};

//--------------------------------------------------------------------------------------
//読み込んだアニメーション付きXファイルの階層情報を元にフレーム情報やメッシュコンテナオブジェクトの生成をするクラス
// ID3DXAllocateHierarchyは派生すること想定して設計されている。
//--------------------------------------------------------------------------------------
class CAllocateHierarchy: public ID3DXAllocateHierarchy{
private:
	CSkinMesh*			m_pSkinMesh = nullptr;

	std::string			m_FolderPath;

public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame) override;
	STDMETHOD(CreateMeshContainer)(THIS_ 
		LPCSTR Name, 
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials, 
		CONST D3DXEFFECTINSTANCE *pEffectInstances, 
		DWORD NumMaterials, 
		CONST DWORD *pAdjacency, 
		LPD3DXSKININFO pSkinInfo, 
		LPD3DXMESHCONTAINER *ppNewMeshContainer) override;
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) override;
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase) override;

		
	// コンストラクタ
	CAllocateHierarchy(const std::string& folderPath, CSkinMesh* skinMesh){
		m_FolderPath = folderPath;
		m_pSkinMesh = skinMesh;
	}
};

}

#endif
