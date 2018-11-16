//#include "../../Include.h"
#include"../../StandardLibraryInclude.h"
#include"../../LibraryInclude.h"
#include"../../SimpleLib/SimpleLib.h"
using namespace SimpleLib;

#include "ToonShader.h"



void ToonShader::Init()
{
	Release();

	//===============================================
	// �G�t�F�N�g(�V�F�[�_)�t�@�C���̃��[�h
	//  ����̓R���p�C���ς݃V�F�[�_�t�@�C��(.cso)��ǂݍ���ł��܂����A
	//  �\�[�X�t�@�C���ł���.fx�����̂܂ܓǂݍ��ނ��Ƃ��ł��܂�(���̏ꍇ�͓ǂݍ���&�R���p�C���ɂȂ�̂ŁA������Ǝ��Ԃ�������)�B
	//  �������.cso�t�@�C����ǂݍ��ނق��𐄏����܂��B
	//===============================================
	LPD3DXBUFFER pErr = nullptr;
	HRESULT hr;
	hr = D3DXCreateEffectFromFile(
		DEV, "Shader/ToonShader.cso",
		nullptr, nullptr, 0, nullptr, &m_Effect, &pErr);

	if(hr != D3D_OK){
		MessageBox(nullptr,(LPCSTR)pErr->GetBufferPointer(),"�V�F�[�_�R���p�C���G���[",MB_OK);
		return;
	}

	// 1x1�̔��e�N�X�`�����쐬
	m_texDot.CreateTexture(DEV, 1,1,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED);
	m_texDot.Fill<UINT>(0xFFFFFFFF);	// ���œh��Ԃ�

	m_texDotNormal.CreateTexture(DEV, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
	m_texDotNormal.Fill<UINT>(0xFF7F7FFF);	// ���œh��Ԃ�

	//�e�N�X�`���쐬
	m_texToonDefault.LoadTexture(DEV, "images/Toon.png");
	m_Effect->SetTexture("g_ToonTex", m_texToonDefault.GetTex());

}

void ToonShader::DrawMesh(CMesh *pMesh)
{
	m_Effect->SetInt("g_CurNumBones", 0);

	// �e�N�j�b�N�I��
	m_Effect->SetTechnique("Tech");
	// �e�N�j�b�N�J�n
	m_Effect->Begin(0, 0);
	// �p�X�J�n
	m_Effect->BeginPass(0);


	// �}�e���A���̐��Ԃ�`��
	for(int i=0; i < pMesh->GetMaterialCnt(); i++ )
	{
		//--------------------------
		// �}�e���A���̃f�[�^���V�F�[�_�փZ�b�g
		//--------------------------
		D3DMATERIAL9&		mate = pMesh->GetMaterials()[i];			// �}�e���A�����擾
		SetMaterial(mate, *pMesh->GetTextures()[i], *pMesh->GetNormalTextures()[i]);

		//--------------------------
		// �V�F�[�_�փf�[�^�̃Z�b�g���m�肳����
		//--------------------------
		// ��������Ă΂Ȃ��ƁA�V�F�[�_���ɂ��܂��p�����[�^���Z�b�g����Ȃ���I
		m_Effect->CommitChanges();

		//--------------------------
		// ���b�V����`��
		//--------------------------
		pMesh->GetMesh()->DrawSubset( i );
	}

	// �p�X�I��
	m_Effect->EndPass();
	// �e�N�j�b�N�I��
	m_Effect->End();
}

void ToonShader::DrawMeshContainer(CBoneController* pBc, D3DXMESHCONTAINER_EX* pMc)
{
	// �X�L�����b�V��
	if (pMc->pSkinInfo)
	{
		// �e�N�j�b�N�I��
		m_Effect->SetTechnique("Tech");
		UINT numPasses;
		// �e�N�j�b�N�J�n
		m_Effect->Begin(&numPasses, 0);
		// �p�X�J�n
		m_Effect->BeginPass(0);

		// �{�[���s���Ɨp�z��
		static std::vector<D3DXMATRIX> workBoneMatArray;
		if (workBoneMatArray.size() < pMc->m_NumPaletteEntries) {
			workBoneMatArray.resize(pMc->m_NumPaletteEntries);
		}

		UINT offID;
		UINT iMatrixIndex;
		LPD3DXBONECOMBINATION pBoneCombination = reinterpret_cast<LPD3DXBONECOMBINATION>(pMc->m_pBoneCombinationBuf->GetBufferPointer());
		for (UINT iAttrib = 0; iAttrib < pMc->m_dwBoneCombiCnt; iAttrib++)
		{
			// �e������s��z���p�ӂ���
			for (UINT iPaletteEntry = 0; iPaletteEntry < pMc->m_NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneCombination[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					offID = pMc->m_BoneFrameArray[iMatrixIndex]->m_OffsetID;
					workBoneMatArray[iPaletteEntry] = pBc->GetBoneTree()[offID]->OffsetLocalMat;
				}
			}

			// �{�[���s��z����Z�b�g
			m_Effect->SetMatrixArray("g_mWArray", &workBoneMatArray[0], pMc->m_NumPaletteEntries);

			// �}�e���A��
			SetMaterial(pMc->pMaterials[ pBoneCombination[iAttrib].AttribId ].MatD3D,
						*pMc->m_Textures[pBoneCombination[iAttrib].AttribId],
						*pMc->m_NormalTextures[pBoneCombination[iAttrib].AttribId]
						);

			// �{�[�����ɂ���Ē��_�V�F�[�_�̏�����I�Ԃ��߂̃t���O���Z�b�g
			m_Effect->SetInt("g_CurNumBones", pMc->m_dwWeight);

			// ��������Ă΂Ȃ��ƁA�V�F�[�_���ɂ��܂��p�����[�^���Z�b�g����Ȃ���I
			m_Effect->CommitChanges();

			// �T�u�Z�b�g��`��
			pMc->MeshData.pMesh->DrawSubset(iAttrib);
		}
		// �e�N�j�b�N�I��
		m_Effect->EndPass();
		// �p�X�I��
		m_Effect->End();
	}
	// �X�^�e�B�b�N���b�V��
	else {
		// �X�^�e�B�b�N�`��
		m_Effect->SetTechnique("Tech");

		UINT passNum;
		m_Effect->Begin(&passNum, 0);
		m_Effect->BeginPass(0);

		// �}�e���A���̐��Ԃ񃋁[�v
		for (UINT i = 0; i < pMc->NumMaterials; i++)
		{
			// �}�e���A���Z�b�g
			SetMaterial( pMc->pMaterials[i].MatD3D, *pMc->m_Textures[i], *pMc->m_NormalTextures[i] );

			// ��������Ă΂Ȃ��ƁA�V�F�[�_���ɂ��܂��p�����[�^���Z�b�g����Ȃ���I
			m_Effect->CommitChanges();

			// ���b�V����`��
			pMc->MeshData.pMesh->DrawSubset(i);
		}

		m_Effect->EndPass();
		m_Effect->End();
	}
}

void ToonShader::SetMaterial(const D3DMATERIAL9& mate, const CTexture& texDiffuse, const CTexture& texNormal)
{
	// �f�B�t���[�Y
	m_Effect->SetVector("g_MateDiffuse", (D3DXVECTOR4*)&mate.Diffuse);

	// �X�y�L����
	m_Effect->SetVector("g_MateSpecular", (D3DXVECTOR4*)&mate.Specular);
		
	// �X�y�L����Power
	m_Effect->SetFloat("g_MateSpePower", mate.Power);

	// �G�~�b�V�u
	m_Effect->SetVector("g_MateEmissive", (D3DXVECTOR4*)&mate.Emissive);

	// ���}�e���A����Ambient�͎g��Ȃ�


	//--------------------------
	// �e�N�X�`�����V�F�[�_�փZ�b�g
	//--------------------------
	LPDIRECT3DTEXTURE9	pTex;

		
	// �e�N�X�`��
	pTex = texDiffuse.GetTex();
	if (pTex == nullptr)pTex = m_texDot.GetTex();	// �e�N�X�`���������Ƃ��́A1x1���e�N�X�`�����g�p

	m_Effect->SetTexture("g_MateDiffuseTex", pTex);

	// �@���}�b�v
	if (m_ForceSetNormalTex) {
		m_Effect->SetTexture("g_MateNormalTex", m_ForceSetNormalTex->GetTex());
	}
	else {
		pTex = texNormal.GetTex();
		if (pTex == nullptr)pTex = m_texDotNormal.GetTex();
		m_Effect->SetTexture("g_MateNormalTex", pTex);
	}
}
