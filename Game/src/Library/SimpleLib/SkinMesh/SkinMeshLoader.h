#ifndef __SkinMeshLoader_h__
#define __SkinMeshLoader_h__

namespace SimpleLib
{

class CSkinMesh;
struct D3DXMESHCONTAINER_EX;

//--------------------------------------------------------------------------------------
// �t���[�����̒ǉ��ŁB�K�w�\���ō\������̂ŌZ��Ƃ��q�̃A�h���X�������Ă�B
//  �|�[���̃W���C���g���������Ă܂���B
//  �����o�ϐ��ł���pMeshContainer�ɂȂɂ������Ă����͕`�悷��̂��B
//--------------------------------------------------------------------------------------
struct D3DXFRAME_EX: public D3DXFRAME
{
	D3DXMATRIX			m_OffsetMat;					// �I�t�Z�b�g�s��
	DWORD				m_OffsetID;						// �s��e�[�u���̃C���f�b�N�X�ԍ�
	D3DXMATRIX			m_DefLocalMat;					// �f�t�H���g�p���ł̃��[�J���s��@���ʂȂ��Ƃ��Ȃ�����g�p���Ȃ�
	D3DXFRAME_EX*		m_pMother;						// �e�{�[���ւ̃A�h���X

	D3DXFRAME_EX() : m_pMother(0), m_OffsetID(-1){
	}

	D3DXMESHCONTAINER_EX* GetMeshContainer(){
		return (D3DXMESHCONTAINER_EX*)pMeshContainer;
	}

	// ���̑�
	// Name�Ƀt���[����
	// TransformationMatrix�ɕϊ��s��
	// pMeshContainer�Ƀ��b�V���R���e�i�ւ̃A�h���X�B
	// pFrameSibling�Ɏ��̌Z��t���[���̃A�h���X
	// pFrameFirstChild�Ɏ�����1�Ԗڂ̎q���t���[���ւ̃A�h���X
};

//--------------------------------------------------------------------------------------
//�e���̃��b�V�����(�`��A�}�e���A���A�e�N�X�`���Ȃ�)��ێ�
// �܂肱���������Ă���t���[����`�悷�邱�ƂɂȂ�B
//--------------------------------------------------------------------------------------
struct D3DXMESHCONTAINER_EX: public D3DXMESHCONTAINER
{
	// �e�N�X�`���z��
	std::vector<CTexture*>		m_Textures;			// �e�N�X�`���z��@�e�N�X�`���{�̂�m_TextureMap�̕��ł��B����͂������ւ̎Q��(�|�C���^)�p
	std::vector<CTexture*>		m_NormalTextures;

	// �X�L�����b�V�����
	LPD3DXMESH					m_pOrigMesh;				// �����H���b�V��

	DWORD						m_dwWeight;					// �d�݂̌�
	DWORD						m_dwBoneCombiCnt;			// �{�[���R���r�l�[�V�����̐�
	LPD3DXBUFFER				m_pBoneCombinationBuf;

	std::vector<D3DXFRAME_EX*>	m_BoneFrameArray;			// �e������t���[���ւ̎Q�Ɣz��B�`�掞�ɂ��̃t���[���̍s����g���B
	DWORD						m_NumPaletteEntries;

	// �\�t�g�E�F�A�X�L�j���O�p
	struct SoftwareSkiningData{
		LPD3DXMESH					pSoftSkinMesh;			// �\�t�g�E�F�A�X�L�j���O�K�p�p���b�V��
		LPD3DXATTRIBUTERANGE		pAttributeTable;
		DWORD						MaterialCnt;			// �{�[���R���r�l�[�V�����̐�

		SoftwareSkiningData() : pSoftSkinMesh(NULL),
								pAttributeTable(NULL),
								MaterialCnt(0)
		{}
	};
	SoftwareSkiningData		m_SoftSkinData;				// �\�t�g�E�F�A�X�L�j���O�p�f�[�^

	D3DXMESHCONTAINER_EX() :
		m_pOrigMesh(NULL),
		m_dwBoneCombiCnt(0),
		m_dwWeight(0),
		m_pBoneCombinationBuf(NULL),
		m_NumPaletteEntries(0)
	{
	}
	// ���̑�
	// Name�Ƀ��b�V����
	// pMaterials�Ƀ}�e���A���̔z��
	// NumMaterials�Ƀ}�e���A����
	// pSkinInfo�ɃX�L�����b�V���̏��
	// pNextMeshContainer�Ɏ��̃��b�V���̃A�h���X�B1�t���[���ɕ����̃��b�V�������Ă�
};

//--------------------------------------------------------------------------------------
//�ǂݍ��񂾃A�j���[�V�����t��X�t�@�C���̊K�w�������Ƀt���[�����⃁�b�V���R���e�i�I�u�W�F�N�g�̐���������N���X
// ID3DXAllocateHierarchy�͔h�����邱�Ƒz�肵�Đ݌v����Ă���B
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

		
	// �R���X�g���N�^
	CAllocateHierarchy(const std::string& folderPath, CSkinMesh* skinMesh){
		m_FolderPath = folderPath;
		m_pSkinMesh = skinMesh;
	}
};

}

#endif
