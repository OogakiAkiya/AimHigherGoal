//==========================================================================================================
// 
// CSurface.h
//
// ����ҁF���c
//
//==========================================================================================================

#ifndef CSurface_h
#define CSurface_h

namespace SimpleLib{

//==================================
// �T�[�t�F�C�X�N���X
//	����Z�o�b�t�@�̍쐬�E�Z�b�g�̗p�r�Ŏg�p
//==================================
class CSurface{
public:
	//==================================================================================
	// �擾
	//==================================================================================
	// �T�[�t�F�C�X�擾
	LPDIRECT3DSURFACE9		GetSur(){return m_lpSur;}
	// ���擾
	D3DSURFACE_DESC*		GetDesc(){return &m_Desc;}

	//==================================================================================
	// �쐬
	//==================================================================================
	// Z�o�b�t�@�Ƃ��č쐬
	BOOL CreateDepthStencil(LPDIRECT3DDEVICE9 lpD3DDev, UINT Width, UINT Height, D3DFORMAT Format = D3DFMT_D24S8, D3DMULTISAMPLE_TYPE MultiSample = D3DMULTISAMPLE_NONE, DWORD MultisampleQuality = 0, BOOL Discard = FALSE);

	//==================================================================================
	// �[�x�o�b�t�@�Ƃ��ăZ�b�g����
	//==================================================================================
	void SetDepthStencil();

	//==================================================================================
	// ���
	//==================================================================================
	void Release();

	// 
	CSurface(){}
	// 
	~CSurface(){
		Release();
	}

private:
	LPDIRECT3DSURFACE9		m_lpSur = nullptr;
	D3DSURFACE_DESC			m_Desc;

	// Direct3D Device�L���p
	LPDIRECT3DDEVICE9		m_lpD3DDev = nullptr;

private:
	// �R�s�[�֎~�p
	CSurface(const CSurface& src){}
	void operator=(const CSurface& src){}
};

}
#endif