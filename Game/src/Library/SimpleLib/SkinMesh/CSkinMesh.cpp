#include"../../StandardLibraryInclude.h"
#include"../../DirectX/D3D.h"

#include "../SimpleLib.h"

using namespace SimpleLib;

//==========================================================================================
//
//
// スキンメッシュクラス
//
//
//==========================================================================================

CSkinMesh::CSkinMesh()
{

}
CSkinMesh::~CSkinMesh()
{
	Release();
}

// AnimationControllerからアニメ情報を取り出し、独自アニメーションデータを作成する。
static void CreateAnimationFromAC(LPD3DXANIMATIONCONTROLLER animController, std::vector<SPtr<CAnimationSet>>& outAnimeList, const std::vector<D3DXFRAME_EX*>& frameArray)
{
	if(animController == NULL){
		return;
	}

	// アニメーションセットから独自アニメーション作成
	for(UINT animLoop=0;animLoop < animController->GetNumAnimationSets();animLoop++)
	{
		LPD3DXKEYFRAMEDANIMATIONSET as;
		animController->GetAnimationSet(animLoop,(LPD3DXANIMATIONSET*)&as);
		int numAnime = as->GetNumAnimations();

		// 新規アニメーション
		SPtr<CAnimationSet> anime = std::make_shared<CAnimationSet>();
		outAnimeList.push_back(anime);

		// 情報設定
		anime->m_AnimeName = as->GetName();						// アニメ名
		anime->m_TicksPerSecond = as->GetSourceTicksPerSecond();	// TicksPerSecound
		double pr = as->GetPeriod();
//		anime->Period = as->GetPeriod(); //※2014/10/03 なんかこの値だとダメなときがあるので、のちに最大フレーム値から計算するように変更

		if (anime->m_AnimeName.substr(0, 2) == "//") {
			outAnimeList.pop_back();
			continue;
		}

		// フレーム(ボーン)の数ぶん確保
		anime->CreateFrameAnimeList(frameArray.size());

		float MaxTime = 0;

		// 全フレームの回転、拡大、座標ゲット
		for(UINT i=0;i<frameArray.size();i++)
		{
			CKeyframeAnime *AnimeFrame = anime->m_FrameAnimeList[i];

			UINT AniIdx;
			#ifdef _DEBUG
			char *frameName = frameArray[i]->Name;
			#endif

			// フレーム名からIndex取得
			if(D3D_OK != as->GetAnimationIndexByName(frameArray[i]->Name, &AniIdx))continue;

			// 回転
			int KeyNum = as->GetNumRotationKeys(AniIdx);
			if(KeyNum > 0){
				D3DXKEY_QUATERNION *lpK_Q = new D3DXKEY_QUATERNION[KeyNum];
				as->GetRotationKeys(AniIdx, lpK_Q);							// クォータニオン配列取得

				for(int k=0;k<KeyNum;k++){
					lpK_Q[k].Value.w *= -1;
				}

				// 回転キーとして登録
				for(int k=0;k<KeyNum;k++){
					CAnimeKey_Quaternion* keyQ = new CAnimeKey_Quaternion();
					keyQ->Time = lpK_Q[k].Time;
					keyQ->Value = lpK_Q[k].Value;
					if(MaxTime < keyQ->Time)MaxTime = keyQ->Time;

					AnimeFrame->m_Rotate.push_back(keyQ);
				}

				delete[] lpK_Q;
			}

			// 拡大
			KeyNum = as->GetNumScaleKeys(AniIdx);
			if(KeyNum > 0){
				D3DXKEY_VECTOR3 *lpK_V = new D3DXKEY_VECTOR3[KeyNum];
				as->GetScaleKeys(AniIdx, lpK_V);

				for(int k=0;k<KeyNum;k++){
					CAnimeKey_Vector3* keyV = new CAnimeKey_Vector3();
					keyV->Time = lpK_V[k].Time;
					keyV->Value = lpK_V[k].Value;

					if(MaxTime < keyV->Time)MaxTime = keyV->Time;

					AnimeFrame->m_Scale.push_back(keyV);
				}

				delete[] lpK_V;
			}

			// 座標
			KeyNum = as->GetNumTranslationKeys(AniIdx);
			if(KeyNum > 0){
				D3DXKEY_VECTOR3 *lpK_V = new D3DXKEY_VECTOR3[KeyNum];
				as->GetTranslationKeys(AniIdx, lpK_V);

				for(int k=0;k<KeyNum;k++){
					CAnimeKey_Vector3* keyV = new CAnimeKey_Vector3();
					keyV->Time = lpK_V[k].Time;
					keyV->Value = lpK_V[k].Value;

					if(MaxTime < keyV->Time)MaxTime = keyV->Time;

					AnimeFrame->m_Pos.push_back(keyV);
				}

				delete[] lpK_V;
			}
		}
		as->Release();

		// 最大フレーム値から、アニメの長を計算
		anime->m_AnimeLen = MaxTime + 1;// / anime->m_TicksPerSecond;
	}
}



//==========================================================================================
// スキンメッシュを読み込む
//==========================================================================================
BOOL CSkinMesh::LoadXFile(const char *lpFileName)
{
	// 解放
	Release();

	// パス、ファイル名抽出
	char szDir  [_MAX_PATH]; 
	char szFname[_MAX_FNAME]; 
	char szExt  [_MAX_EXT]; 
	_splitpath( lpFileName, NULL, szDir, szFname, szExt);
	m_FilePath = szDir;
	m_FileName = szFname;
	m_FileName += szExt;

	// 読み込み
	LPD3DXANIMATIONCONTROLLER animController;
	CAllocateHierarchy Alloc(m_FilePath, this);
	D3DXFRAME_EX* lpFrameRoot;
	HRESULT hr = D3DXLoadMeshHierarchyFromX(lpFileName, D3DXMESH_MANAGED, DEV, &Alloc, NULL, (LPD3DXFRAME*)&lpFrameRoot, &animController);
	
	if(lpFrameRoot == NULL)return FALSE;

	// フレーム配列＆メッシュ存在フレーム配列作成 & 各フレームの親を更新
	std::function<void(D3DXFRAME_EX*, D3DXFRAME_EX*)> recCreateFrameArray_And_MeshArray = [this, &recCreateFrameArray_And_MeshArray](D3DXFRAME_EX *pFrame, D3DXFRAME_EX *pParentFrame)
	{
		// 親設定
		pFrame->m_pMother = pParentFrame;

		// 全フレーム用配列
		m_FrameArray.push_back(pFrame);

		// メッシュコンテナ配列に追加
		D3DXMESHCONTAINER* pMc = pFrame->pMeshContainer;
		while (pMc) {
			m_MeshContainerArray.push_back((D3DXMESHCONTAINER_EX*)pMc);

			pMc = pMc->pNextMeshContainer;
		}

		// 子
		if (pFrame->pFrameFirstChild != NULL)
		{
			recCreateFrameArray_And_MeshArray((D3DXFRAME_EX*)pFrame->pFrameFirstChild, pFrame);
		}
		// 兄弟
		if (pFrame->pFrameSibling != NULL)
		{
			recCreateFrameArray_And_MeshArray((D3DXFRAME_EX*)pFrame->pFrameSibling, pParentFrame);
		}
	};

	recCreateFrameArray_And_MeshArray(lpFrameRoot, NULL);

	// 全フレームのDefWorldMat算出
	CalcBoneMatrix();

	// 全フレームのOffsetMat作成
	for (UINT i = 0; i<m_MeshContainerArray.size(); i++) {
		D3DXMESHCONTAINER_EX* pMeshContainer = m_MeshContainerArray[i];
		// スキン情報
		if (pMeshContainer->pSkinInfo) {
			pMeshContainer->m_BoneFrameArray.clear();

			UINT cBones = pMeshContainer->pSkinInfo->GetNumBones();	// このメッシュに影響を受けるボーン数
			for (UINT iBone = 0; iBone < cBones; iBone++)
			{
				// フレーム内から同じ名前のフレームを検索
				for (UINT Idx = 0; Idx<m_FrameArray.size(); Idx++) {
					if (strcmp(pMeshContainer->pSkinInfo->GetBoneName(iBone), m_FrameArray[Idx]->Name) == 0) {
						pMeshContainer->m_BoneFrameArray.push_back(m_FrameArray[Idx]);

						// Offset行列
						m_FrameArray[Idx]->m_OffsetMat = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));

						break;
					}
				}
			}
		}
	}

	// フレーム配列にオフセットID作成 & Offset作成
	for (UINT Idx = 0; Idx<m_FrameArray.size(); Idx++) {
		m_FrameArray[Idx]->m_OffsetID = Idx;	// idx
											// OffsetMatが単位行列の奴は作成
		if (D3DXMatrixIsIdentity(&m_FrameArray[Idx]->m_OffsetMat)) {
			D3DXMatrixInverse(&m_FrameArray[Idx]->m_OffsetMat, NULL, &m_FrameArray[Idx]->m_DefLocalMat);
		}
	}

	// アニメーションコントローラから独自アニメーション情報作成
	CreateAnimationFromAC(animController, m_AnimeList, m_FrameArray);

	SAFE_RELEASE(animController);

	return TRUE;
}

//==========================================================================================
// スキンメッシュを解放
//==========================================================================================
void CSkinMesh::Release()
{

	if( m_FrameArray.size() > 0 ){
		CAllocateHierarchy Alloc("", nullptr);
		D3DXFrameDestroy( m_FrameArray[0], &Alloc );
	}

	m_FrameArray.clear();
	m_MeshContainerArray.clear();

	m_AnimeList.clear();

	m_FileName.clear();
	m_FilePath.clear();
}

void CSkinMesh::CalcBoneMatrix()
{
	CMatrix mI;
	recCalcBoneMatrix(m_FrameArray[0], &mI);
}

void CSkinMesh::recCalcBoneMatrix(D3DXFRAME_EX *pFrame, D3DXMATRIX *ParentMat)
{
	pFrame->m_DefLocalMat = pFrame->TransformationMatrix * (*ParentMat);

	// 子
	if (pFrame->pFrameFirstChild != NULL)
	{
		recCalcBoneMatrix((D3DXFRAME_EX*)pFrame->pFrameFirstChild, &pFrame->m_DefLocalMat);
	}
	// 兄弟
	if (pFrame->pFrameSibling != NULL)
	{
		recCalcBoneMatrix((D3DXFRAME_EX*)pFrame->pFrameSibling, ParentMat);
	}
}

bool CSkinMesh::ComputeTangentFrame()
{
	for(UINT i=0;i<m_MeshContainerArray.size();i++){
		D3DXMESHCONTAINER_EX* pMeshContainer = m_MeshContainerArray[i];
		while(pMeshContainer){
			//頂点形式定義
			D3DVERTEXELEMENT9 decl[] =
			{
				{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
				{0, 12, D3DDECLTYPE_FLOAT1,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,  0},
				{0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
				{0, 20, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
				{0, 32, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
				{0, 40, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
				{0, 52, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
				D3DDECL_END()
			};

			// 現在の頂点形式を取得し、D3DDECLUSAGE_BLENDWEIGHTの部分を一致させる
			D3DVERTEXELEMENT9 elem[MAX_FVF_DECL_SIZE];
			pMeshContainer->MeshData.pMesh->GetDeclaration( elem );
			for(int el=0;;el++){
				if(elem[el].Stream == 0xFF){
					break;
				}
				// BlendWeightの型をあわし、Offsetもずらす
				else if(elem[el].Usage == D3DDECLUSAGE_BLENDWEIGHT){
					int addOffset = 0;
					// Float2
					if( elem[el].Type == D3DDECLTYPE_FLOAT2){
						decl[1].Type = D3DDECLTYPE_FLOAT2;
						addOffset = 4;
					}
					// Float3
					else if( elem[el].Type == D3DDECLTYPE_FLOAT3){
						decl[1].Type = D3DDECLTYPE_FLOAT3;
						addOffset = 8;
					}
					// Float4
					else if( elem[el].Type == D3DDECLTYPE_FLOAT3){
						decl[1].Type = D3DDECLTYPE_FLOAT4;
						addOffset = 12;
					}
					// Offsetずらし
					for(int k=2;;k++){
						if(decl[k].Stream == 0xFF)break;
						decl[k].Offset += addOffset;
					}
				}
			}

			// 新たな頂点形式でメッシュを作成
			LPD3DXMESH lpTmpMesh;
			if(FAILED(pMeshContainer->MeshData.pMesh->CloneMesh( pMeshContainer->MeshData.pMesh->GetOptions(),decl, DEV, &lpTmpMesh ))){
				return false;
			}
			// 現在のメッシュを削除
			SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

			if(pMeshContainer->pSkinInfo){
				// セットしなくてもシェーダ描画では問題ない。
				// セットするとソフトスキニングができなくなる。これってID3DXSkinInfo::UpdateSkinnedMesh()用？
//				pMeshContainer->pSkinInfo->SetDeclaration( decl );	
			}

			// 従法線、接線ベクトルを算出した新たなメッシュを作成
			D3DXComputeTangentFrameEx(	lpTmpMesh,
										D3DDECLUSAGE_TEXCOORD,
										0,
										D3DDECLUSAGE_TANGENT,
										0,
										D3DDECLUSAGE_BINORMAL,
										0,
										D3DDECLUSAGE_NORMAL,
										0,
										0,
										NULL,
										0.01f,    //ボケ具合。値をおおきくするとぼけなくなる
										0.25f,
										0.01f,
										&pMeshContainer->MeshData.pMesh,
										NULL
										);
			// 一時メッシュ削除										
			SAFE_RELEASE(lpTmpMesh);

			// 次のメッシュ
			pMeshContainer = (D3DXMESHCONTAINER_EX*)pMeshContainer->pNextMeshContainer;
		}
	}
	/*
	//頂点データ定義
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,  0},
		{0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		{0, 36, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0, 48, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	*/


	return true;
}

CTexture* CSkinMesh::LoadTexture_UseMap(const char* filename)
{
	CTexture* tex = nullptr;
	// ファイル名をキーに、テクスチャ管理マップから検索・取得
	auto it = m_TextureMap.find(filename);
	if (it == m_TextureMap.end()) {	// 存在しない
									// 新規作成
		tex = &m_TextureMap[filename];
		// テクスチャ読み込み
		tex->LoadTexture(DEV, filename);
	}
	else {
		// 発見したやつをそのまま使用
		tex = &(*it).second;
	}
	return tex;
}
