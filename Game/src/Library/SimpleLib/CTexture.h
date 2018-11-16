//==========================================================================================================
// 
// CTexture.h
//
// ����ҁF���c
//
//==========================================================================================================

#ifndef CTexture_h
#define CTexture_h

namespace SimpleLib{

//==============================================================================================
// �e�N�X�`���N���X
//	�e�N�X�`���̓ǂݍ��݁ART�e�N�X�`���̍쐬�E�Z�b�g�Ȃǂ��\
//�@����͎����ōs����
//==============================================================================================
class CTexture{
public:
	//===============================================================================================
	// ���擾
	//===============================================================================================
	// �e�N�X�`���擾
	LPDIRECT3DTEXTURE9		GetTex() const {return m_lpTex;}
	// �T�[�t�F�C�X�擾
	LPDIRECT3DSURFACE9		GetSur() const {return m_lpSur;}
	// RECT�擾
	const RECT*				GetRect() const {return &m_rc;}
	// �t�@�C����
	std::string				GetName() const {return m_strName;}
	// ���
	const D3DXIMAGE_INFO*	GetInfo() const {return &m_Info;}

	//===============================================================================================
	// �ǂݍ��݁E�쐬
	//===============================================================================================

	// �ȈՔŃt�@�C������e�N�X�`���ǂݍ���
	// [����] nonPow2 �c �ǂݍ��񂾃e�N�X�`����2�ׂ̂���Ɋg�k���Ȃ��悤�ɂ���
	bool LoadTexture(LPDIRECT3DDEVICE9 lpD3DDev, LPCTSTR pSrcFile, bool nonPow2 = false);
	
	// �t�@�C������e�N�X�`���ǂݍ���(�g��)
	// ������D3DXCreateTextureFromFileEx()�Ƃقړ���
	bool LoadTextureEx(
						LPDIRECT3DDEVICE9 lpD3DDev,
						LPCTSTR pSrcFile,
						UINT Width,
						UINT Height,
						UINT MipLevels,
						DWORD Usage,
						D3DFORMAT Format,
						D3DPOOL Pool,
						DWORD Filter,
						DWORD MipFilter,
						D3DCOLOR ColorKey,
						PALETTEENTRY *pPalette
						);

	// �e�N�X�`���쐬
	bool CreateTexture(
						LPDIRECT3DDEVICE9 lpD3DDev,
						UINT Width,
						UINT Height,
						UINT MipLevels,
						DWORD Usage,
						D3DFORMAT Format,
						D3DPOOL Pool
						);

	// �����_�[�^�[�Q�b�g�e�N�X�`���Ƃ��č쐬
	bool CreateRenderTarget(LPDIRECT3DDEVICE9 lpD3DDev, UINT Width,	UINT Height, D3DFORMAT Format = D3DFMT_A8R8G8B8){
		return CreateTexture(lpD3DDev, Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT);
	}

	//===============================================================================================
	// �e�N�X�`�����f�o�C�X�ɐݒ�
	//===============================================================================================
	void SetTexture(DWORD StageNo=0);

	//===============================================================================================
	// ���
	//===============================================================================================
	void Release();
	
	//===============================================================================================
	// �����_�[�^�[�Q�b�g�ɐݒ�
	//===============================================================================================
	HRESULT SetRenderTarget(DWORD Index=0);

	//===============================================================================================
	// ���ډ��H
	//===============================================================================================
	template<class T>
	bool Fill(T fill, DWORD LockFlag=0);	// �h��Ԃ�

	// 
	CTexture() {
		ZeroMemory(&m_Info, sizeof(m_Info));
		SetRect(&m_rc, 0, 0, 0, 0);
	}
	//
	~CTexture() {
		Release();
	}

private:

	LPDIRECT3DTEXTURE9	m_lpTex = nullptr;		// �e�N�X�`��
	LPDIRECT3DSURFACE9	m_lpSur = nullptr;		// �T�[�t�F�C�X
	D3DXIMAGE_INFO		m_Info;					// ���
	RECT				m_rc;					// RECT
	std::string			m_strName;				// �t�@�C����

	LPDIRECT3DDEVICE9	m_lpD3DDev = nullptr;		// Direct3D Device�L���p

private:
	// �R�s�[�֎~�p
	CTexture(const CTexture& src){}
	void operator=(const CTexture& src){}
};



// 
template<class T>
bool CTexture::Fill(T fill,DWORD LockFlag)
{
	D3DLOCKED_RECT LockedRect;
	// ���b�N���āA�s�N�Z���̐擪�A�h���X�擾
	if(SUCCEEDED(m_lpTex->LockRect(0, &LockedRect, nullptr, LockFlag))){
		//==================================
		//�����ŉ摜�f�[�^��]��
		//==================================
		T* p1 = (T*)LockedRect.pBits;
		T pitch = LockedRect.Pitch / sizeof(T);
		for ( UINT y = 0; y < m_Info.Height ; y++ )
		{
			for ( UINT x = 0; x < m_Info.Width; x++ )
			{
				p1[x] = fill;
			}
			p1 += pitch;    //���̍s
		}
	
		m_lpTex->UnlockRect(0);
		return true;
	}
	return false;
}

}

#endif
