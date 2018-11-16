#ifndef CBoneController_h
#define CBoneController_h

namespace SimpleLib {

//===========================================================================================
//
// �{�[�����쑀��N���X
//
//===========================================================================================
class CBoneController{
public:
	struct BoneNode;

	//=================================================================================
	// ���擾
	//=================================================================================
	std::shared_ptr<CSkinMesh> 			GetSkinMesh(){return m_pMesh;}					// �X�L�����b�V���N���X�擾
	std::vector<BoneNode*>&				GetBoneTree(){return m_BoneTree;}				// �t���[���e�[�u���擾

	//=================================================================================
	// �ݒ�A���
	//=================================================================================
	// �X�L�����b�V���̐ݒ� & �{�[���𓯂��\���ō\�z����
	BOOL SetSkinMesh(std::shared_ptr<CSkinMesh> lpCSkinMesh);

	// ���
	void Release();

	//=================================================================================
	// ���̃��f���p�ɃA�j���[�^������������
	// ��Root�{�[���Ɋւ��ẮA�����g���b�N�u�����h�������ɂȂ�܂�
	// animator	�c ����������A�j���[�^
	//=================================================================================
	void InitAnimator(CAnimator& animator);

	//=================================================================================
	void ResetDefaultTransMat();

	// mat���x�[�X�ɁATrack�����g�p���āA�S�Ẵ{�[�����v�Z����B
	// Track���̐ݒ�ɂ��A���[�V�����u�����h���s���B
	void CalcBoneMatrix();					// �S�W���C���g�̍s����v�Z
		void recCalcBoneMatrix(BoneNode *treenode,D3DXMATRIX *ParentMat);

	// �X�L�����b�V���`��
	// mat : �X�^�e�B�b�N���b�V�����Ɏg����s��(SetTransform�����)
	void Draw();
		void DrawMeshContainer(D3DXMESHCONTAINER_EX* pMc);

	// �t���[���e�[�u������w�肵�����O�̃{�[���̏����擾 ���O�����݂��Ȃ��ꍇ��NULL���Ԃ�
	BoneNode* SearchBone(const char *BoneName){
		for(UINT i=0;i<m_BoneTree.size();i++){
			if(strcmp(m_BoneTree[i]->lpFrame->Name,BoneName) == 0){
				return m_BoneTree[i];
			}
		}
		return NULL;
	}

protected:
	std::shared_ptr<CSkinMesh>			m_pMesh;				// ���ƂȂ�X�L���w�t�@�C���N���X�̃|�C���^

	// �t���[���c���[���z��
	std::vector<BoneNode*>				m_BoneTree;				// [0]��Root �c���[�\���ł��邪�A1�����z��Ƃ��Ă��A�N�Z�X�\�B

public:
	CBoneController()
	{
		m_pMesh = NULL;
	}

	~CBoneController(){
		Release();
	}

	//-----------------------------
	// �{�[���m�[�h
	//-----------------------------
	struct BoneNode {
		D3DXFRAME_EX*			lpFrame;	// D3DFRAME�ւ̃A�h���X
		BoneNode*				Mother;	// �e
		std::vector<BoneNode*>	Child;	// �q�z��

		int				Level;			// �K�w Root��0
		CMatrix			TransMat;		// �{�[�����[�J���s��
		CMatrix			LocalMat;		// ���[�J���s��
		CMatrix			OffsetLocalMat;	// Offset * LocalMat�K�p��(�`��p)

		bool			Disable;		// �v�Z����

		// �{�[��Index�擾
		int GetBoneIndex() {
			if (lpFrame == NULL)return -1;
			return lpFrame->m_OffsetID;
		}

		BoneNode() :
			Mother(0),
			Level(0),
			lpFrame(NULL),
			Disable(false)
		{
		}
	};

private:
	// �R�s�[�֎~�p
	CBoneController(const CBoneController& src){}
	void operator=(const CBoneController& src){}
};

}
#endif
