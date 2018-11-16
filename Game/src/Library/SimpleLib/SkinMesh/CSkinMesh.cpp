#include"../../StandardLibraryInclude.h"
#include"../../DirectX/D3D.h"

#include "../SimpleLib.h"

using namespace SimpleLib;

//==========================================================================================
//
//
// �X�L�����b�V���N���X
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

// AnimationController����A�j���������o���A�Ǝ��A�j���[�V�����f�[�^���쐬����B
static void CreateAnimationFromAC(LPD3DXANIMATIONCONTROLLER animController, std::vector<SPtr<CAnimationSet>>& outAnimeList, const std::vector<D3DXFRAME_EX*>& frameArray)
{
	if(animController == NULL){
		return;
	}

	// �A�j���[�V�����Z�b�g����Ǝ��A�j���[�V�����쐬
	for(UINT animLoop=0;animLoop < animController->GetNumAnimationSets();animLoop++)
	{
		LPD3DXKEYFRAMEDANIMATIONSET as;
		animController->GetAnimationSet(animLoop,(LPD3DXANIMATIONSET*)&as);
		int numAnime = as->GetNumAnimations();

		// �V�K�A�j���[�V����
		SPtr<CAnimationSet> anime = std::make_shared<CAnimationSet>();
		outAnimeList.push_back(anime);

		// ���ݒ�
		anime->m_AnimeName = as->GetName();						// �A�j����
		anime->m_TicksPerSecond = as->GetSourceTicksPerSecond();	// TicksPerSecound
		double pr = as->GetPeriod();
//		anime->Period = as->GetPeriod(); //��2014/10/03 �Ȃ񂩂��̒l���ƃ_���ȂƂ�������̂ŁA�̂��ɍő�t���[���l����v�Z����悤�ɕύX

		if (anime->m_AnimeName.substr(0, 2) == "//") {
			outAnimeList.pop_back();
			continue;
		}

		// �t���[��(�{�[��)�̐��Ԃ�m��
		anime->CreateFrameAnimeList(frameArray.size());

		float MaxTime = 0;

		// �S�t���[���̉�]�A�g��A���W�Q�b�g
		for(UINT i=0;i<frameArray.size();i++)
		{
			CKeyframeAnime *AnimeFrame = anime->m_FrameAnimeList[i];

			UINT AniIdx;
			#ifdef _DEBUG
			char *frameName = frameArray[i]->Name;
			#endif

			// �t���[��������Index�擾
			if(D3D_OK != as->GetAnimationIndexByName(frameArray[i]->Name, &AniIdx))continue;

			// ��]
			int KeyNum = as->GetNumRotationKeys(AniIdx);
			if(KeyNum > 0){
				D3DXKEY_QUATERNION *lpK_Q = new D3DXKEY_QUATERNION[KeyNum];
				as->GetRotationKeys(AniIdx, lpK_Q);							// �N�H�[�^�j�I���z��擾

				for(int k=0;k<KeyNum;k++){
					lpK_Q[k].Value.w *= -1;
				}

				// ��]�L�[�Ƃ��ēo�^
				for(int k=0;k<KeyNum;k++){
					CAnimeKey_Quaternion* keyQ = new CAnimeKey_Quaternion();
					keyQ->Time = lpK_Q[k].Time;
					keyQ->Value = lpK_Q[k].Value;
					if(MaxTime < keyQ->Time)MaxTime = keyQ->Time;

					AnimeFrame->m_Rotate.push_back(keyQ);
				}

				delete[] lpK_Q;
			}

			// �g��
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

			// ���W
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

		// �ő�t���[���l����A�A�j���̒����v�Z
		anime->m_AnimeLen = MaxTime + 1;// / anime->m_TicksPerSecond;
	}
}



//==========================================================================================
// �X�L�����b�V����ǂݍ���
//==========================================================================================
BOOL CSkinMesh::LoadXFile(const char *lpFileName)
{
	// ���
	Release();

	// �p�X�A�t�@�C�������o
	char szDir  [_MAX_PATH]; 
	char szFname[_MAX_FNAME]; 
	char szExt  [_MAX_EXT]; 
	_splitpath( lpFileName, NULL, szDir, szFname, szExt);
	m_FilePath = szDir;
	m_FileName = szFname;
	m_FileName += szExt;

	// �ǂݍ���
	LPD3DXANIMATIONCONTROLLER animController;
	CAllocateHierarchy Alloc(m_FilePath, this);
	D3DXFRAME_EX* lpFrameRoot;
	HRESULT hr = D3DXLoadMeshHierarchyFromX(lpFileName, D3DXMESH_MANAGED, DEV, &Alloc, NULL, (LPD3DXFRAME*)&lpFrameRoot, &animController);
	
	if(lpFrameRoot == NULL)return FALSE;

	// �t���[���z�񁕃��b�V�����݃t���[���z��쐬 & �e�t���[���̐e���X�V
	std::function<void(D3DXFRAME_EX*, D3DXFRAME_EX*)> recCreateFrameArray_And_MeshArray = [this, &recCreateFrameArray_And_MeshArray](D3DXFRAME_EX *pFrame, D3DXFRAME_EX *pParentFrame)
	{
		// �e�ݒ�
		pFrame->m_pMother = pParentFrame;

		// �S�t���[���p�z��
		m_FrameArray.push_back(pFrame);

		// ���b�V���R���e�i�z��ɒǉ�
		D3DXMESHCONTAINER* pMc = pFrame->pMeshContainer;
		while (pMc) {
			m_MeshContainerArray.push_back((D3DXMESHCONTAINER_EX*)pMc);

			pMc = pMc->pNextMeshContainer;
		}

		// �q
		if (pFrame->pFrameFirstChild != NULL)
		{
			recCreateFrameArray_And_MeshArray((D3DXFRAME_EX*)pFrame->pFrameFirstChild, pFrame);
		}
		// �Z��
		if (pFrame->pFrameSibling != NULL)
		{
			recCreateFrameArray_And_MeshArray((D3DXFRAME_EX*)pFrame->pFrameSibling, pParentFrame);
		}
	};

	recCreateFrameArray_And_MeshArray(lpFrameRoot, NULL);

	// �S�t���[����DefWorldMat�Z�o
	CalcBoneMatrix();

	// �S�t���[����OffsetMat�쐬
	for (UINT i = 0; i<m_MeshContainerArray.size(); i++) {
		D3DXMESHCONTAINER_EX* pMeshContainer = m_MeshContainerArray[i];
		// �X�L�����
		if (pMeshContainer->pSkinInfo) {
			pMeshContainer->m_BoneFrameArray.clear();

			UINT cBones = pMeshContainer->pSkinInfo->GetNumBones();	// ���̃��b�V���ɉe�����󂯂�{�[����
			for (UINT iBone = 0; iBone < cBones; iBone++)
			{
				// �t���[�������瓯�����O�̃t���[��������
				for (UINT Idx = 0; Idx<m_FrameArray.size(); Idx++) {
					if (strcmp(pMeshContainer->pSkinInfo->GetBoneName(iBone), m_FrameArray[Idx]->Name) == 0) {
						pMeshContainer->m_BoneFrameArray.push_back(m_FrameArray[Idx]);

						// Offset�s��
						m_FrameArray[Idx]->m_OffsetMat = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));

						break;
					}
				}
			}
		}
	}

	// �t���[���z��ɃI�t�Z�b�gID�쐬 & Offset�쐬
	for (UINT Idx = 0; Idx<m_FrameArray.size(); Idx++) {
		m_FrameArray[Idx]->m_OffsetID = Idx;	// idx
											// OffsetMat���P�ʍs��̓z�͍쐬
		if (D3DXMatrixIsIdentity(&m_FrameArray[Idx]->m_OffsetMat)) {
			D3DXMatrixInverse(&m_FrameArray[Idx]->m_OffsetMat, NULL, &m_FrameArray[Idx]->m_DefLocalMat);
		}
	}

	// �A�j���[�V�����R���g���[������Ǝ��A�j���[�V�������쐬
	CreateAnimationFromAC(animController, m_AnimeList, m_FrameArray);

	SAFE_RELEASE(animController);

	return TRUE;
}

//==========================================================================================
// �X�L�����b�V�������
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

	// �q
	if (pFrame->pFrameFirstChild != NULL)
	{
		recCalcBoneMatrix((D3DXFRAME_EX*)pFrame->pFrameFirstChild, &pFrame->m_DefLocalMat);
	}
	// �Z��
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
			//���_�`����`
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

			// ���݂̒��_�`�����擾���AD3DDECLUSAGE_BLENDWEIGHT�̕�������v������
			D3DVERTEXELEMENT9 elem[MAX_FVF_DECL_SIZE];
			pMeshContainer->MeshData.pMesh->GetDeclaration( elem );
			for(int el=0;;el++){
				if(elem[el].Stream == 0xFF){
					break;
				}
				// BlendWeight�̌^�����킵�AOffset�����炷
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
					// Offset���炵
					for(int k=2;;k++){
						if(decl[k].Stream == 0xFF)break;
						decl[k].Offset += addOffset;
					}
				}
			}

			// �V���Ȓ��_�`���Ń��b�V�����쐬
			LPD3DXMESH lpTmpMesh;
			if(FAILED(pMeshContainer->MeshData.pMesh->CloneMesh( pMeshContainer->MeshData.pMesh->GetOptions(),decl, DEV, &lpTmpMesh ))){
				return false;
			}
			// ���݂̃��b�V�����폜
			SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

			if(pMeshContainer->pSkinInfo){
				// �Z�b�g���Ȃ��Ă��V�F�[�_�`��ł͖��Ȃ��B
				// �Z�b�g����ƃ\�t�g�X�L�j���O���ł��Ȃ��Ȃ�B�������ID3DXSkinInfo::UpdateSkinnedMesh()�p�H
//				pMeshContainer->pSkinInfo->SetDeclaration( decl );	
			}

			// �]�@���A�ڐ��x�N�g�����Z�o�����V���ȃ��b�V�����쐬
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
										0.01f,    //�{�P��B�l��������������Ƃڂ��Ȃ��Ȃ�
										0.25f,
										0.01f,
										&pMeshContainer->MeshData.pMesh,
										NULL
										);
			// �ꎞ���b�V���폜										
			SAFE_RELEASE(lpTmpMesh);

			// ���̃��b�V��
			pMeshContainer = (D3DXMESHCONTAINER_EX*)pMeshContainer->pNextMeshContainer;
		}
	}
	/*
	//���_�f�[�^��`
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
	// �t�@�C�������L�[�ɁA�e�N�X�`���Ǘ��}�b�v���猟���E�擾
	auto it = m_TextureMap.find(filename);
	if (it == m_TextureMap.end()) {	// ���݂��Ȃ�
									// �V�K�쐬
		tex = &m_TextureMap[filename];
		// �e�N�X�`���ǂݍ���
		tex->LoadTexture(DEV, filename);
	}
	else {
		// ��������������̂܂܎g�p
		tex = &(*it).second;
	}
	return tex;
}
