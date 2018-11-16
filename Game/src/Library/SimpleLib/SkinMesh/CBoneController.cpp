#include"../../StandardLibraryInclude.h"
#include"../../DirectX/D3D.h"

#include "../SimpleLib.h"

using namespace SimpleLib;

//====================================================================================================
//
// CBoneController
//
//====================================================================================================
BOOL CBoneController::SetSkinMesh(std::shared_ptr<CSkinMesh> lpCSkinMesh)
{
	Release();

	if(lpCSkinMesh == NULL){
		return FALSE;
	}

	m_pMesh = lpCSkinMesh;	// CSkinMeshのアドレス登録

	if(m_pMesh->GetRoot() == NULL)return FALSE;

	// ボーンツリー作成
	std::function<void(D3DXFRAME_EX*, BoneNode*, int)> recCreateBoneTree = [this, &recCreateBoneTree](D3DXFRAME_EX *pFrame, BoneNode *boneNode, int Level)
	{

		// ツリーに登録
		m_BoneTree.push_back(boneNode);

		// 情報セット
		boneNode->lpFrame = pFrame;
		boneNode->Level = Level;

		// 子を再帰
		if (pFrame->pFrameFirstChild != NULL)
		{
			D3DXFRAME_EX *frame = (D3DXFRAME_EX*)pFrame->pFrameFirstChild;
			while (frame != NULL) {
				// 子登録
				BoneNode *bn = new BoneNode();
				bn->Mother = boneNode;
				boneNode->Child.push_back(bn);

				// 再帰
				recCreateBoneTree(frame, bn, Level + 1);

				// 次へ
				frame = (D3DXFRAME_EX*)frame->pFrameSibling;
			}
		}
	};


	recCreateBoneTree(m_pMesh->GetRoot(), new BoneNode(), 0);

	// とりあえず、変換行列系もコピーしとく
	for(UINT i=0;i<m_BoneTree.size();i++){
		m_BoneTree[i]->TransMat = m_BoneTree[i]->lpFrame->TransformationMatrix;
		m_BoneTree[i]->LocalMat = m_BoneTree[i]->lpFrame->m_DefLocalMat;
	}

	return TRUE;
}

void CBoneController::ResetDefaultTransMat()
{
	for(UINT i=0;i<m_BoneTree.size();i++){
		m_BoneTree[i]->TransMat = m_BoneTree[i]->lpFrame->TransformationMatrix;
	}
}

void CBoneController::CalcBoneMatrix()
{
	if(m_BoneTree.empty())return;

	CMatrix mI;
	recCalcBoneMatrix(m_BoneTree[0], &mI);
}

	void CBoneController::recCalcBoneMatrix(BoneNode *treenode,D3DXMATRIX *ParentMat)
	{
		// 計算
		if( !treenode->Disable ){
			// ワールド行列作成
			D3DXMatrixMultiply(&treenode->LocalMat, &treenode->TransMat, ParentMat);
			// オフセット行列合成版も作成
			D3DXMatrixMultiply(&treenode->OffsetLocalMat, &treenode->lpFrame->m_OffsetMat, &treenode->LocalMat);
		}
		else{
			// ワールド行列作成
			D3DXMatrixMultiply(&treenode->LocalMat, &treenode->lpFrame->TransformationMatrix, ParentMat);
			// オフセット行列合成版も作成
			D3DXMatrixMultiply(&treenode->OffsetLocalMat, &treenode->lpFrame->m_OffsetMat, &treenode->LocalMat);
		}

		// 子再帰
		for(UINT i=0;i<treenode->Child.size();i++)
		{
			recCalcBoneMatrix(treenode->Child[i], &treenode->LocalMat);
		}
	}

void CBoneController::Draw()
{
	if(m_pMesh == NULL)return;

	// すべてのメッシュを描画
	for(UINT i=0;i<GetSkinMesh()->GetMeshContainerArray().size();i++)
	{
		DrawMeshContainer( GetSkinMesh()->GetMeshContainerArray()[i]);
	}
}

void CBoneController::DrawMeshContainer(D3DXMESHCONTAINER_EX* pMc)
{
	//スキンメッシュの場合
	if(pMc->pSkinInfo)
	{
		// ソフトウェアスキニング

		// 影響ボーン配列作成
		static std::vector<CMatrix>				sWorkMatrixTbl;	// ソフトウェアスキニング用の作業用行列配列

		CMatrix mW;
		DEV->GetTransform(D3DTS_WORLD, &mW);

		CMatrix mI;

		UINT offID;
		DWORD cBones = pMc->pSkinInfo->GetNumBones();
		if (sWorkMatrixTbl.size() < cBones)sWorkMatrixTbl.resize(cBones);

		for (UINT bn = 0; bn < cBones; bn++){
			offID = pMc->m_BoneFrameArray[bn]->m_OffsetID;
			sWorkMatrixTbl[bn] = m_BoneTree[offID]->OffsetLocalMat * mW;
		}

		// ワールド変換行列は単位行列で
		// スキニング適用して、頂点更新
		PBYTE pbVerticesSrc;		// オリジナルメッシュの頂点バッファポインタ
		PBYTE pbVerticesDest;		// 現行メッシュの頂点バッファポインタ
		DEV->SetTransform(D3DTS_WORLD, &mI);
		pMc->m_pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);		// オリジナルメッシュの頂点バッファをロック
		pMc->m_SoftSkinData.pSoftSkinMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);	// 現行メッシュの頂点バッファをロック
			
			// 現在の行列に基づいて、ターゲットの頂点にソフトウェア スキニングを適用する
			pMc->pSkinInfo->UpdateSkinnedMesh(&sWorkMatrixTbl[0], NULL, pbVerticesSrc, pbVerticesDest);

		pMc->m_pOrigMesh->UnlockVertexBuffer();
		pMc->m_SoftSkinData.pSoftSkinMesh->UnlockVertexBuffer();

		// 描画
		for (UINT iAttrib = 0; iAttrib < pMc->m_SoftSkinData.MaterialCnt; iAttrib++)
		{
			// マテリアル設定
			DEV->SetMaterial(&(pMc->pMaterials[pMc->m_SoftSkinData.pAttributeTable[iAttrib].AttribId].MatD3D));
			// テクスチャ設定
			DEV->SetTexture(0, pMc->m_Textures[pMc->m_SoftSkinData.pAttributeTable[iAttrib].AttribId]->GetTex());
			// サブセット描画
			pMc->m_SoftSkinData.pSoftSkinMesh->DrawSubset(pMc->m_SoftSkinData.pAttributeTable[iAttrib].AttribId);
		}
	}
	// スタティックメッシュの場合
	else{
		for (UINT i = 0; i < pMc->NumMaterials; i++)
		{
			DEV->SetMaterial( &pMc->pMaterials[i].MatD3D );
			DEV->SetTexture( 0, pMc->m_Textures[i]->GetTex() );
			pMc->MeshData.pMesh->DrawSubset(i);
		}
	}
}

void CBoneController::Release()
{
	for(UINT i=0;i<m_BoneTree.size();i++){
		SAFE_DELETE(m_BoneTree[i]);
	}
	m_BoneTree.clear();

	m_pMesh = NULL;
}

void CBoneController::InitAnimator(CAnimator& animator)
{
	// アニメータ初期化
	animator.Init();

	// アニメリストコピー
	if (m_pMesh) {
		animator.CopyAnimationList(m_pMesh->GetAnimeList());
	}

	// フレームアニメデータ構築
	animator.ClearRefMatrix();
	for (UINT i = 0; i < m_BoneTree.size(); i++) {
		bool bBlendRootBone = (i == 0 ? false : true);	// Rootボーンのみ、複数トラックブレンドを無効にする

		animator.AddRefMatrix(&m_BoneTree[i]->TransMat, bBlendRootBone, bBlendRootBone, bBlendRootBone);
	}
}
