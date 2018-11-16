#ifndef CSkinMesh_h
#define CSkinMesh_h
class D3D;
namespace SimpleLib{

#define MAX_BONE 26

//=================================================================================
// スキンメッシュ対応メッシュクラス
//  CSkinObjectクラスにセットして使用する。
//=================================================================================
class CSkinMesh{
public:
	//=================================================================================
	// 情報取得
	//=================================================================================
	std::vector<D3DXFRAME_EX*>&			GetFrameArray(){return m_FrameArray;}					// フレーム参照用配列取得
	std::vector<D3DXMESHCONTAINER_EX*>	GetMeshContainerArray(){return m_MeshContainerArray;}
	std::vector<SPtr<CAnimationSet>>&	GetAnimeList(){return m_AnimeList;}					// アニメーションリストを取得
	D3DXFRAME_EX*						GetRoot(){return (m_FrameArray.size() == 0 ? NULL : m_FrameArray[0]);}						// ルートフレーム取得
	std::string&						GetFileName(){return m_FileName;}
	std::string&						GetFilePath(){return m_FilePath;}

	//=================================================================================
	// Xファイル読み込み
	//=================================================================================
	BOOL LoadXFile(const char *lpFileName);	// Xファイル読み込み

	//=================================================================================
	// 解放
	//=================================================================================
	void Release();

	// 全メッシュの全頂点にに従法線、接線を追加
	bool ComputeTangentFrame();

	// ボーン名検索 見つからない場合はNULL
	D3DXFRAME_EX* SearchBone(const char *Name){
		for(UINT i=0;i<m_FrameArray.size();i++){
			// 文字列判定
			if(strcmp(m_FrameArray[i]->Name,Name) == 0){
				return m_FrameArray[i];
			}
		}
		return NULL;
	}

	// 全FrameのDefWorldMatを算出
	void CalcBoneMatrix();
		void recCalcBoneMatrix(D3DXFRAME_EX *pFrame, D3DXMATRIX *ParentMat);


	// 
	CSkinMesh();
	~CSkinMesh();

	CTexture* LoadTexture_UseMap(const char* filename);

protected:
	std::string					m_FilePath;			// ファイルパス
	std::string					m_FileName;			// ファイル名

	// フレーム参照用配列
	std::vector<D3DXFRAME_EX*>			m_FrameArray;			// 全フレーム参照配列
	std::vector<D3DXMESHCONTAINER_EX*>	m_MeshContainerArray;	// メッシュコンテナー配列　ここに全メッシュコンテナの参照が入っている

	// アニメーション
	std::vector<SPtr<CAnimationSet>>	m_AnimeList;		// アニメーションリスト

	// 同名のテクスチャを重複して読み込んでメモリを無駄にするのを防ぐためのもの
	std::map<std::string, CTexture>		m_TextureMap;


private:
	// コピー禁止用
	CSkinMesh(const CSkinMesh& src){}
	void operator=(const CSkinMesh& src){}
};


}
#endif
