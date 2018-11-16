#ifndef CBoneController_h
#define CBoneController_h

namespace SimpleLib {

//===========================================================================================
//
// ボーン操作操作クラス
//
//===========================================================================================
class CBoneController{
public:
	struct BoneNode;

	//=================================================================================
	// 情報取得
	//=================================================================================
	std::shared_ptr<CSkinMesh> 			GetSkinMesh(){return m_pMesh;}					// スキンメッシュクラス取得
	std::vector<BoneNode*>&				GetBoneTree(){return m_BoneTree;}				// フレームテーブル取得

	//=================================================================================
	// 設定、解放
	//=================================================================================
	// スキンメッシュの設定 & ボーンを同じ構成で構築する
	BOOL SetSkinMesh(std::shared_ptr<CSkinMesh> lpCSkinMesh);

	// 解放
	void Release();

	//=================================================================================
	// このモデル用にアニメータを初期化する
	// ※Rootボーンに関しては、複数トラックブレンドが無効になります
	// animator	… 初期化するアニメータ
	//=================================================================================
	void InitAnimator(CAnimator& animator);

	//=================================================================================
	void ResetDefaultTransMat();

	// matをベースに、Track情報を使用して、全てのボーンを計算する。
	// Track情報の設定により、モーションブレンドも行う。
	void CalcBoneMatrix();					// 全ジョイントの行列を計算
		void recCalcBoneMatrix(BoneNode *treenode,D3DXMATRIX *ParentMat);

	// スキンメッシュ描画
	// mat : スタティックメッシュ時に使われる行列(SetTransformされる)
	void Draw();
		void DrawMeshContainer(D3DXMESHCONTAINER_EX* pMc);

	// フレームテーブルから指定した名前のボーンの情報を取得 名前が存在しない場合はNULLが返る
	BoneNode* SearchBone(const char *BoneName){
		for(UINT i=0;i<m_BoneTree.size();i++){
			if(strcmp(m_BoneTree[i]->lpFrame->Name,BoneName) == 0){
				return m_BoneTree[i];
			}
		}
		return NULL;
	}

protected:
	std::shared_ptr<CSkinMesh>			m_pMesh;				// 元となるスキンＸファイルクラスのポインタ

	// フレームツリー＆配列
	std::vector<BoneNode*>				m_BoneTree;				// [0]がRoot ツリー構造であるが、1次元配列としてもアクセス可能。

public:
	CBoneController()
	{
		m_pMesh = NULL;
	}

	~CBoneController(){
		Release();
	}

	//-----------------------------
	// ボーンノード
	//-----------------------------
	struct BoneNode {
		D3DXFRAME_EX*			lpFrame;	// D3DFRAMEへのアドレス
		BoneNode*				Mother;	// 親
		std::vector<BoneNode*>	Child;	// 子配列

		int				Level;			// 階層 Rootが0
		CMatrix			TransMat;		// ボーンローカル行列
		CMatrix			LocalMat;		// ローカル行列
		CMatrix			OffsetLocalMat;	// Offset * LocalMat適用後(描画用)

		bool			Disable;		// 計算無効

		// ボーンIndex取得
		int GetBoneIndex() {
			if (lpFrame == NULL)return -1;
			return lpFrame->m_OffsetID;
		}

		BoneNode() :
			Mother(0),
			Level(0),
			lpFrame(NULL),
			Disable(false)
		{
		}
	};

private:
	// コピー禁止用
	CBoneController(const CBoneController& src){}
	void operator=(const CBoneController& src){}
};

}
#endif
