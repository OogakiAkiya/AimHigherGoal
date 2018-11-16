//==========================================================================================================
// 
// CMesh.h
//
// ����ҁF���c
//
//==========================================================================================================

#ifndef CMesh_h
#define CMesh_h

namespace SimpleLib{

//=================================================================
// ���b�V���N���X
//	X�t�@�C�����f����ǂݍ��݁E�`��(�Œ�@�\)���\�B
//�@����͎����ōs����
//=================================================================
class CMesh{
public:
	//===============================================================================================
	// ���擾�n
	//===============================================================================================
	// ���b�V���擾
	LPD3DXMESH					GetMesh(){return m_pMesh;}

	// �}�e���A�����擾
	int							GetMaterialCnt(){return (int)m_Materials.size();}

	// �e�N�X�`���z��擾
	std::vector<CTexture*>&		GetTextures() { return m_Textures; }

	// �@���}�b�v�e�N�X�`���z��擾
	std::vector<CTexture*>&		GetNormalTextures() { return m_NormalTextures; }

	// �}�e���A���e�[�u���擾
	std::vector<D3DMATERIAL9>&	GetMaterials(){return m_Materials;}	

	// �אڐ��f�[�^�擾
	LPD3DXBUFFER				GetAdjacency(){return m_pAdjacency;}

	//===============================================================================================
	// �ǂݍ���
	//===============================================================================================
	// X�t�@�C���ǂݍ���
	bool LoadXFile(LPDIRECT3DDEVICE9 lpD3DDev,const char *lpFileName);

	//===============================================================================================
	// ���
	//===============================================================================================
	// ���b�V�����
	void Release();

	//===============================================================================================
	// �`��֌W
	//===============================================================================================
	// �Œ�@�\�ł̃��b�V���`��
	void Draw();

	// 
	CMesh() {}
	//
	~CMesh() {
		Release();
	}

private:

	//---------------
	// ���b�V��
	//---------------
	LPD3DXMESH						m_pMesh = nullptr;
	
	//---------------
	// �}�e���A���֌W
	//---------------
	std::vector<D3DMATERIAL9>		m_Materials;	// �}�e���A���z��

	std::vector<CTexture*>			m_Textures;			// �e�N�X�`���z��@�e�N�X�`���{�̂�m_TextureMap�̕��ł��B����͂������ւ̎Q��(�|�C���^)�p
	std::vector<CTexture*>			m_NormalTextures;

	// �����̃e�N�X�`�����d�����ēǂݍ���Ń������𖳑ʂɂ���̂�h�����߂̂���
	std::map<std::string, CTexture>	m_TextureMap;

	//---------------
	// ���̑�
	//---------------
	LPD3DXBUFFER					m_pAdjacency = nullptr;		// �אڐ��f�[�^

	std::string						m_FileName;					// �t�@�C����

	LPDIRECT3DDEVICE9				m_lpD3DDev = nullptr;		// Direct3D Device�L���p

	CTexture* LoadTexture_UseMap(const char* filename);

private:
	// �R�s�[�֎~�p
	CMesh(const CMesh& src){}
	void operator=(const CMesh& src){}
};

}
#endif
