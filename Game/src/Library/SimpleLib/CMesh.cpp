#include "SimpleLib.h"

using namespace SimpleLib;


bool CMesh::LoadXFile(LPDIRECT3DDEVICE9 lpD3DDev,const char *lpFileName)
{
	Release();

	// �f�o�C�X�L��
	m_lpD3DDev = lpD3DDev;
	m_lpD3DDev->AddRef();	// �Q�ƃJ�E���^���P���₷

	// �p�X�A�t�@�C��������
	std::string Path;
	char szDir  [_MAX_PATH];	// �t�H���_
	char szFname[_MAX_FNAME];	// �t�@�C����(�g���q������)
	char szExt  [_MAX_EXT];		// �g���q
	_splitpath( lpFileName, nullptr, szDir, szFname, szExt);
	Path = szDir;

	m_FileName = szFname;
	m_FileName += szExt;

	HRESULT hr;

	LPD3DXBUFFER	pD3DXMtrlBuffer;

	// X�t�@�C���ǂݍ���
	DWORD mateNum;
	hr = D3DXLoadMeshFromX( lpFileName, 
							D3DXMESH_MANAGED, 
							lpD3DDev, 
							&m_pAdjacency, 
							&pD3DXMtrlBuffer, // �}�e���A���f�[�^���i�[���邽�߂̕ϐ����w��
							nullptr,
							&mateNum, // �}�e���A�������i�[���邽�߂̕ϐ����w��
							&m_pMesh);	// ���b�V���f�[�^���i�[���邽�߂̕ϐ����w��
	if(FAILED(hr)) return false;

	// ���b�V���̃}�e���A���̑����Ɋ�Â��āA�}�e���A���z����쐬
	m_Textures.resize(mateNum);
	m_NormalTextures.resize(mateNum);
	m_Materials.resize(mateNum);

	// pD3DXMtrlBuffer ����A������e�N�X�`���[�̏���ǂݎ��
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	// �}�e���A���R�s�[�A�e�N�X�`���ǂݍ���
	std::string strTexFile;
	for(DWORD i=0; i<mateNum; i++ ){
		// �ގ�
		m_Materials[i] = d3dxMaterials[i].MatD3D;			// �����̃R�s�[
		m_Materials[i].Ambient = m_Materials[i].Diffuse;	// Ambient��Diffuse�̃R�s�[���g�p

		// �e�N�X�`���� Path + �t�@�C����
		strTexFile = Path;
		if (d3dxMaterials[i].pTextureFilename) { strTexFile += d3dxMaterials[i].pTextureFilename; }

		// �������e�N�X�`�����d�����ēǂݍ��ނ̂�h�~���邽�߁Astd::map�Ńe�N�X�`�����Ǘ����Ă��܂�

		// �t�@�C�������L�[�ɁA�e�N�X�`���Ǘ��}�b�v���猟���E�擾/�ǂݍ���
		m_Textures[i] = LoadTexture_UseMap(strTexFile.c_str());

		// �@���}�b�v
		std::string normalTexFile = ConvertExtFileName(strTexFile, "normal");
		m_NormalTextures[i] = LoadTexture_UseMap(normalTexFile.c_str());

//		ConvertExtFileName
	}

	// ��Ɨp�}�e���A�� �o�b�t�@�J��
	SAFE_RELEASE(pD3DXMtrlBuffer);

	return true;
}
CTexture* CMesh::LoadTexture_UseMap(const char* filename)
{
	CTexture* tex = nullptr;
	// �t�@�C�������L�[�ɁA�e�N�X�`���Ǘ��}�b�v���猟���E�擾
	auto it = m_TextureMap.find(filename);
	if (it == m_TextureMap.end()) {	// ���݂��Ȃ�
									// �V�K�쐬
		tex = &m_TextureMap[filename];
		// �e�N�X�`���ǂݍ���
		tex->LoadTexture(m_lpD3DDev, filename);
	}
	else {
		// ��������������̂܂܎g�p
		tex = &(*it).second;
	}
	return tex;
}


void CMesh::Release()
{

	m_TextureMap.clear();

	m_Textures.clear();
	m_NormalTextures.clear();
	m_Materials.clear();

	// �אږʃf�[�^�폜
	SAFE_RELEASE(m_pAdjacency);

	// ���b�V�� �I�u�W�F�N�g���
	SAFE_RELEASE(m_pMesh);

	// �f�o�C�X�̎Q�ƃJ�E���^�����炷
	SAFE_RELEASE(m_lpD3DDev);

	m_FileName = "";
}

void CMesh::Draw()
{
	if (m_pMesh == nullptr)return;

	// �}�e���A���̐��Ԃ񃋁[�v
	for(UINT i=0; i < m_Materials.size(); i++ ){
		// i�Ԗڂ̃}�e���A����ݒ�
		m_lpD3DDev->SetMaterial( &m_Materials[i] );
		// i�Ԗڂ̃e�N�X�`����ݒ�
		m_lpD3DDev->SetTexture( 0, m_Textures[i]->GetTex() );

		// i�Ԗڂ̃}�e���A���̃��b�V����`��
		m_pMesh->DrawSubset( i );
	}
}
