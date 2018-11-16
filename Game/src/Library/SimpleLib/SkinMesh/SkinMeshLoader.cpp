#include"../../StandardLibraryInclude.h"
#include"../../DirectX/D3D.h"

#include "../SimpleLib.h"

using namespace SimpleLib;


//=============================================================================
// CAllocateHierarchy
//=============================================================================
//--------------------------------------------------------------------------------------
// �t���[�����̓��b�V���Ɏg�p���閼�O�p�̃��������m��
//--------------------------------------------------------------------------------------
static HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName )
{
	UINT cbLength;

	if( Name != NULL )
	{
		cbLength = (UINT)strlen(Name) + 1;
		*pNewName = new CHAR[cbLength];
		if(*pNewName == NULL){
			return E_OUTOFMEMORY;
		}
		memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
	}
	else
	{
		// *pNewName = NULL;

		// �Œ�ł�""������
		*pNewName = new CHAR[1];
		if(*pNewName == NULL){
			return E_OUTOFMEMORY;
		}
		*pNewName[0] = '\0';
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
// �t���[�����쐬
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME *ppNewFrame )
{
	HRESULT hr = S_OK;
	D3DXFRAME_EX *pFrame;

	*ppNewFrame = NULL;

	pFrame = new D3DXFRAME_EX;
	if (pFrame == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	hr = AllocateName(Name, &pFrame->Name);
	if(FAILED(hr))
		goto e_Exit;

	// ���̑�
	pFrame->m_OffsetID = 0xFFFFFFFF;
	D3DXMatrixIdentity(&pFrame->m_OffsetMat);

	D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	pFrame->pMeshContainer		= NULL;
	pFrame->pFrameSibling		= NULL;
	pFrame->pFrameFirstChild	= NULL;

	*ppNewFrame = pFrame;
	pFrame = NULL;

e_Exit:
	SAFE_DELETE(pFrame);
	return hr;
}

static HRESULT CreateSkinningData(D3DXMESHCONTAINER_EX *pMeshContainer)
{
	HRESULT hr = S_OK;
	D3DCAPS9 d3dCaps;
	DEV->GetDeviceCaps(&d3dCaps);

	// �X�L�����b�V���̏�񂪖����ꍇ�͏I��
	if (pMeshContainer->pSkinInfo == NULL)return hr;

	// �f�[�^����x���
	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->m_pBoneCombinationBuf);
	SAFE_RELEASE(pMeshContainer->m_SoftSkinData.pSoftSkinMesh);

	// �V�F�[�_�p�̃X�L�j���O�f�[�^�쐬
	{
		// Get palette size
		// First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
		// (96 - 9) /3 i.e. Maximum constant count - used constants 
		UINT MaxMatrices = MAX_BONE;
		pMeshContainer->m_NumPaletteEntries = min(MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones());
		DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
		Flags |= D3DXMESH_MANAGED;

		SAFE_RELEASE(pMeshContainer->MeshData.pMesh);


		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh(
			pMeshContainer->m_pOrigMesh,
			Flags,
			pMeshContainer->m_NumPaletteEntries,
			pMeshContainer->pAdjacency,
			NULL, NULL, NULL,
			&pMeshContainer->m_dwWeight,
			&pMeshContainer->m_dwBoneCombiCnt,
			&pMeshContainer->m_pBoneCombinationBuf,
			&pMeshContainer->MeshData.pMesh);
		if (FAILED(hr))
			goto e_Exit;

		// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
		DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
		if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
		{
			LPD3DXMESH pMesh;
			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, DEV, &pMesh);
			if (!FAILED(hr))
			{
				pMeshContainer->MeshData.pMesh->Release();
				pMeshContainer->MeshData.pMesh = pMesh;
				pMesh = NULL;
			}
		}
		D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
		LPD3DVERTEXELEMENT9 pDeclCur;
		hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
		if (FAILED(hr))
			goto e_Exit;
		// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
		//   NOTE: this cannot be done with CloneMesh, that would convert 
		//   the UBYTE4 data to float and then to D3DCOLOR
		//          this is more of a "cast" operation
		pDeclCur = pDecl;
		while (pDeclCur->Stream != 0xff)
		{
			if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
				pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
			pDeclCur++;
		}
		hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
		if (FAILED(hr)) {
			goto e_Exit;
		}
	}


	// �\�t�g�E�F�A�X�L�j���O�f�[�^�쐬
	{
		// �`���Ɨp���b�V���쐬
		hr = pMeshContainer->m_pOrigMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshContainer->m_pOrigMesh->GetFVF(), DEV, &pMeshContainer->m_SoftSkinData.pSoftSkinMesh);
		if (FAILED(hr)) {
			goto e_Exit;
		}

		hr = pMeshContainer->m_SoftSkinData.pSoftSkinMesh->GetAttributeTable(NULL, &pMeshContainer->m_SoftSkinData.MaterialCnt);
		if (FAILED(hr)) {
			goto e_Exit;
		}
		SAFE_DELETE_ARRAY(pMeshContainer->m_SoftSkinData.pAttributeTable);
		pMeshContainer->m_SoftSkinData.pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->m_SoftSkinData.MaterialCnt];
		if (pMeshContainer->m_SoftSkinData.pAttributeTable == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}
		hr = pMeshContainer->m_SoftSkinData.pSoftSkinMesh->GetAttributeTable(pMeshContainer->m_SoftSkinData.pAttributeTable, NULL);
		if (FAILED(hr)) {
			goto e_Exit;
		}

	}

e_Exit:
	return hr;
}

//--------------------------------------------------------------------------------------
// ���b�V���R���e�i�쐬
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(
	LPCSTR Name, 
	CONST D3DXMESHDATA *pMeshData,
	CONST D3DXMATERIAL *pMaterials,
	CONST D3DXEFFECTINSTANCE *pEffectInstances, 
	DWORD NumMaterials, 
	CONST DWORD *pAdjacency, 
	LPD3DXSKININFO pSkinInfo, 
	LPD3DXMESHCONTAINER *ppNewMeshContainer) 
{
	HRESULT hr;
	D3DXMESHCONTAINER_EX *pMeshContainer = NULL;
	UINT NumFaces;
	UINT iMaterial;
	LPDIRECT3DDEVICE9 pd3dDevice = NULL;

	LPD3DXMESH pMesh = NULL;

	*ppNewMeshContainer = NULL;

	// this sample does not handle patch meshes, so fail when one is found
	if (pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// ���b�V���f�[�^���擾
	pMesh = pMeshData->pMesh;

	// this sample does not FVF compatible meshes, so fail when one is found
	if (pMesh->GetFVF() == 0)
	{
		hr = E_FAIL;
		goto e_Exit;
	}

	// ���b�V���R���e�i�쐬
	pMeshContainer = new D3DXMESHCONTAINER_EX;
	if (pMeshContainer == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}

	// ���b�V���R���e�i�����R�s�[
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
		goto e_Exit;        

	pMesh->GetDevice(&pd3dDevice);		// �f�o�C�X�擾
	NumFaces = pMesh->GetNumFaces();	// �ʐ��擾

	// ���Y���b�V�����@���������Ȃ��ꍇ�͖@����ǉ�����
	if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		// �@����t���ă��b�V���𕡐�����
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
									pMesh->GetFVF() | D3DFVF_NORMAL, 
									pd3dDevice, &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
			goto e_Exit;

		// get the new pMesh pointer back out of the mesh container to use
		// NOTE: we do not release pMesh because we do not have a reference to it yet
		pMesh = pMeshContainer->MeshData.pMesh;

		// now generate the normals for the pmesh
		D3DXComputeNormals( pMesh, NULL );
	}
	else  // if no normals, just add a reference to the mesh for the mesh container
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		pMesh->AddRef();
	}

	// �אږʃf�[�^
	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
	if (pMeshContainer->pAdjacency == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);

	//���b�V���̃}�e���A���ݒ�
	pMeshContainer->NumMaterials = NumMaterials;
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->m_Textures.resize(pMeshContainer->NumMaterials);
	pMeshContainer->m_NormalTextures.resize(pMeshContainer->NumMaterials);

	// �}�e���A���R�s�[
	if (NumMaterials > 0)            
	{
		std::string texName;
		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++)
		{
			// �}�e���A���R�s�[
			pMeshContainer->pMaterials[iMaterial].MatD3D = pMaterials[iMaterial].MatD3D;
			pMeshContainer->pMaterials[iMaterial].MatD3D.Ambient = pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse;	// Ambient��Diffuse�̃R�s�[���g��
			pMeshContainer->pMaterials[iMaterial].pTextureFilename = nullptr;

			// �e�N�X�`����
			std::string fileName = (pMaterials[iMaterial].pTextureFilename == NULL ? "" : pMaterials[iMaterial].pTextureFilename);
			texName = m_FolderPath + fileName;

			// �e�N�X�`��
//			pMeshContainer->m_Textures[iMaterial].LoadTexture(DEV, texName.c_str());
			pMeshContainer->m_Textures[iMaterial] = m_pSkinMesh->LoadTexture_UseMap(texName.c_str());

			// �@���}�b�v
			std::string normalTexFile = ConvertExtFileName(texName, "normal");
			pMeshContainer->m_NormalTextures[iMaterial] = m_pSkinMesh->LoadTexture_UseMap(normalTexFile.c_str());

		}
	}
	else // �}�e���A�����S���Ȃ��Ȃ�A�f�t�H���g���g�p
	{
		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 1.0f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	// �X�L�����������Ă���ꍇ�i�X�L�����b�V���ŗL�̏����j
	if (pSkinInfo != NULL)
	{
		// �܂��ŏ��ɁA�f�[�^��SkinInfo�ƃI���W�i���̃��b�V���ɕۑ�����܂��B
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		pMeshContainer->m_pOrigMesh = pMesh;
		pMesh->AddRef();

		hr = CreateSkinningData( pMeshContainer );
		if (FAILED(hr)){
			goto e_Exit;
		}
	}
	// �X�^�e�B�b�N���b�V���̏ꍇ
	else{
		pMeshContainer->pSkinInfo = NULL;

		pMeshContainer->m_pOrigMesh = pMesh;
		pMesh->AddRef();
	}

	//���[�J���ɐ����������b�V���R���e�i�[���Ăяo�����ɃR�s�[����
	pMeshContainer->pEffects = NULL;
	pMeshContainer->pNextMeshContainer = NULL;
	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;

e_Exit:
	SAFE_RELEASE(pd3dDevice);

	// call Destroy function to properly clean up the memory allocated 
	if (pMeshContainer != NULL)
	{
		DestroyMeshContainer(pMeshContainer);
	}

	return hr;
}

//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	D3DXFRAME_EX* p = (D3DXFRAME_EX*)pFrameToFree;

	SAFE_DELETE_ARRAY( p->Name );
	SAFE_DELETE( p );

	return S_OK; 
}




//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	UINT iMaterial;
	D3DXMESHCONTAINER_EX *pMeshContainer = (D3DXMESHCONTAINER_EX*)pMeshContainerBase;

	for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
	{
		SAFE_DELETE( pMeshContainer->pMaterials[iMaterial].pTextureFilename );
	}

	SAFE_DELETE_ARRAY( pMeshContainer->Name );
	SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
	SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
//	SAFE_DELETE_ARRAY( pMeshContainer->m_Textures );
	pMeshContainer->m_Textures.clear();
	pMeshContainer->m_NormalTextures.clear();

	SAFE_RELEASE( pMeshContainer->m_SoftSkinData.pSoftSkinMesh );
	SAFE_DELETE_ARRAY( pMeshContainer->m_SoftSkinData.pAttributeTable );

	pMeshContainer->m_BoneFrameArray.clear();
	SAFE_RELEASE( pMeshContainer->m_pBoneCombinationBuf );
	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pSkinInfo );
	SAFE_RELEASE( pMeshContainer->m_pOrigMesh );
	D3DXMESHCONTAINER_EX* pMC = (D3DXMESHCONTAINER_EX*)pMeshContainer;
	SAFE_DELETE( pMC );
	return S_OK;
}
