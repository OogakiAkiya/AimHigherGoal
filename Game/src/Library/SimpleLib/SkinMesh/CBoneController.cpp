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

	m_pMesh = lpCSkinMesh;	// CSkinMesh�̃A�h���X�o�^

	if(m_pMesh->GetRoot() == NULL)return FALSE;

	// �{�[���c���[�쐬
	std::function<void(D3DXFRAME_EX*, BoneNode*, int)> recCreateBoneTree = [this, &recCreateBoneTree](D3DXFRAME_EX *pFrame, BoneNode *boneNode, int Level)
	{

		// �c���[�ɓo�^
		m_BoneTree.push_back(boneNode);

		// ���Z�b�g
		boneNode->lpFrame = pFrame;
		boneNode->Level = Level;

		// �q���ċA
		if (pFrame->pFrameFirstChild != NULL)
		{
			D3DXFRAME_EX *frame = (D3DXFRAME_EX*)pFrame->pFrameFirstChild;
			while (frame != NULL) {
				// �q�o�^
				BoneNode *bn = new BoneNode();
				bn->Mother = boneNode;
				boneNode->Child.push_back(bn);

				// �ċA
				recCreateBoneTree(frame, bn, Level + 1);

				// ����
				frame = (D3DXFRAME_EX*)frame->pFrameSibling;
			}
		}
	};


	recCreateBoneTree(m_pMesh->GetRoot(), new BoneNode(), 0);

	// �Ƃ肠�����A�ϊ��s��n���R�s�[���Ƃ�
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
		// �v�Z
		if( !treenode->Disable ){
			// ���[���h�s��쐬
			D3DXMatrixMultiply(&treenode->LocalMat, &treenode->TransMat, ParentMat);
			// �I�t�Z�b�g�s�񍇐��ł��쐬
			D3DXMatrixMultiply(&treenode->OffsetLocalMat, &treenode->lpFrame->m_OffsetMat, &treenode->LocalMat);
		}
		else{
			// ���[���h�s��쐬
			D3DXMatrixMultiply(&treenode->LocalMat, &treenode->lpFrame->TransformationMatrix, ParentMat);
			// �I�t�Z�b�g�s�񍇐��ł��쐬
			D3DXMatrixMultiply(&treenode->OffsetLocalMat, &treenode->lpFrame->m_OffsetMat, &treenode->LocalMat);
		}

		// �q�ċA
		for(UINT i=0;i<treenode->Child.size();i++)
		{
			recCalcBoneMatrix(treenode->Child[i], &treenode->LocalMat);
		}
	}

void CBoneController::Draw()
{
	if(m_pMesh == NULL)return;

	// ���ׂẴ��b�V����`��
	for(UINT i=0;i<GetSkinMesh()->GetMeshContainerArray().size();i++)
	{
		DrawMeshContainer( GetSkinMesh()->GetMeshContainerArray()[i]);
	}
}

void CBoneController::DrawMeshContainer(D3DXMESHCONTAINER_EX* pMc)
{
	//�X�L�����b�V���̏ꍇ
	if(pMc->pSkinInfo)
	{
		// �\�t�g�E�F�A�X�L�j���O

		// �e���{�[���z��쐬
		static std::vector<CMatrix>				sWorkMatrixTbl;	// �\�t�g�E�F�A�X�L�j���O�p�̍�Ɨp�s��z��

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

		// ���[���h�ϊ��s��͒P�ʍs���
		// �X�L�j���O�K�p���āA���_�X�V
		PBYTE pbVerticesSrc;		// �I���W�i�����b�V���̒��_�o�b�t�@�|�C���^
		PBYTE pbVerticesDest;		// ���s���b�V���̒��_�o�b�t�@�|�C���^
		DEV->SetTransform(D3DTS_WORLD, &mI);
		pMc->m_pOrigMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc);		// �I���W�i�����b�V���̒��_�o�b�t�@�����b�N
		pMc->m_SoftSkinData.pSoftSkinMesh->LockVertexBuffer(0, (LPVOID*)&pbVerticesDest);	// ���s���b�V���̒��_�o�b�t�@�����b�N
			
			// ���݂̍s��Ɋ�Â��āA�^�[�Q�b�g�̒��_�Ƀ\�t�g�E�F�A �X�L�j���O��K�p����
			pMc->pSkinInfo->UpdateSkinnedMesh(&sWorkMatrixTbl[0], NULL, pbVerticesSrc, pbVerticesDest);

		pMc->m_pOrigMesh->UnlockVertexBuffer();
		pMc->m_SoftSkinData.pSoftSkinMesh->UnlockVertexBuffer();

		// �`��
		for (UINT iAttrib = 0; iAttrib < pMc->m_SoftSkinData.MaterialCnt; iAttrib++)
		{
			// �}�e���A���ݒ�
			DEV->SetMaterial(&(pMc->pMaterials[pMc->m_SoftSkinData.pAttributeTable[iAttrib].AttribId].MatD3D));
			// �e�N�X�`���ݒ�
			DEV->SetTexture(0, pMc->m_Textures[pMc->m_SoftSkinData.pAttributeTable[iAttrib].AttribId]->GetTex());
			// �T�u�Z�b�g�`��
			pMc->m_SoftSkinData.pSoftSkinMesh->DrawSubset(pMc->m_SoftSkinData.pAttributeTable[iAttrib].AttribId);
		}
	}
	// �X�^�e�B�b�N���b�V���̏ꍇ
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
	// �A�j���[�^������
	animator.Init();

	// �A�j�����X�g�R�s�[
	if (m_pMesh) {
		animator.CopyAnimationList(m_pMesh->GetAnimeList());
	}

	// �t���[���A�j���f�[�^�\�z
	animator.ClearRefMatrix();
	for (UINT i = 0; i < m_BoneTree.size(); i++) {
		bool bBlendRootBone = (i == 0 ? false : true);	// Root�{�[���̂݁A�����g���b�N�u�����h�𖳌��ɂ���

		animator.AddRefMatrix(&m_BoneTree[i]->TransMat, bBlendRootBone, bBlendRootBone, bBlendRootBone);
	}
}
