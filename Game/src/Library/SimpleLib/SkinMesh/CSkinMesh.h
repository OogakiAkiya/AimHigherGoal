#ifndef CSkinMesh_h
#define CSkinMesh_h
class D3D;
namespace SimpleLib{

#define MAX_BONE 26

//=================================================================================
// �X�L�����b�V���Ή����b�V���N���X
//  CSkinObject�N���X�ɃZ�b�g���Ďg�p����B
//=================================================================================
class CSkinMesh{
public:
	//=================================================================================
	// ���擾
	//=================================================================================
	std::vector<D3DXFRAME_EX*>&			GetFrameArray(){return m_FrameArray;}					// �t���[���Q�Ɨp�z��擾
	std::vector<D3DXMESHCONTAINER_EX*>	GetMeshContainerArray(){return m_MeshContainerArray;}
	std::vector<SPtr<CAnimationSet>>&	GetAnimeList(){return m_AnimeList;}					// �A�j���[�V�������X�g���擾
	D3DXFRAME_EX*						GetRoot(){return (m_FrameArray.size() == 0 ? NULL : m_FrameArray[0]);}						// ���[�g�t���[���擾
	std::string&						GetFileName(){return m_FileName;}
	std::string&						GetFilePath(){return m_FilePath;}

	//=================================================================================
	// X�t�@�C���ǂݍ���
	//=================================================================================
	BOOL LoadXFile(const char *lpFileName);	// X�t�@�C���ǂݍ���

	//=================================================================================
	// ���
	//=================================================================================
	void Release();

	// �S���b�V���̑S���_�ɂɏ]�@���A�ڐ���ǉ�
	bool ComputeTangentFrame();

	// �{�[�������� ������Ȃ��ꍇ��NULL
	D3DXFRAME_EX* SearchBone(const char *Name){
		for(UINT i=0;i<m_FrameArray.size();i++){
			// �����񔻒�
			if(strcmp(m_FrameArray[i]->Name,Name) == 0){
				return m_FrameArray[i];
			}
		}
		return NULL;
	}

	// �SFrame��DefWorldMat���Z�o
	void CalcBoneMatrix();
		void recCalcBoneMatrix(D3DXFRAME_EX *pFrame, D3DXMATRIX *ParentMat);


	// 
	CSkinMesh();
	~CSkinMesh();

	CTexture* LoadTexture_UseMap(const char* filename);

protected:
	std::string					m_FilePath;			// �t�@�C���p�X
	std::string					m_FileName;			// �t�@�C����

	// �t���[���Q�Ɨp�z��
	std::vector<D3DXFRAME_EX*>			m_FrameArray;			// �S�t���[���Q�Ɣz��
	std::vector<D3DXMESHCONTAINER_EX*>	m_MeshContainerArray;	// ���b�V���R���e�i�[�z��@�����ɑS���b�V���R���e�i�̎Q�Ƃ������Ă���

	// �A�j���[�V����
	std::vector<SPtr<CAnimationSet>>	m_AnimeList;		// �A�j���[�V�������X�g

	// �����̃e�N�X�`�����d�����ēǂݍ���Ń������𖳑ʂɂ���̂�h�����߂̂���
	std::map<std::string, CTexture>		m_TextureMap;


private:
	// �R�s�[�֎~�p
	CSkinMesh(const CSkinMesh& src){}
	void operator=(const CSkinMesh& src){}
};


}
#endif
