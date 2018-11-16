#include "SimpleLib.h"

using namespace SimpleLib;

bool CTexture::LoadTexture(LPDIRECT3DDEVICE9 lpD3DDev, LPCTSTR pSrcFile, bool nonPow2)
{
	Release();

	// �f�o�C�X�L��
	m_lpD3DDev = lpD3DDev;
	m_lpD3DDev->AddRef();	// �Q�ƃJ�E���^���P���₷

	//
	m_strName = pSrcFile;

	UINT sizeFlag = D3DX_DEFAULT;
	if (nonPow2) {
		sizeFlag = D3DX_DEFAULT_NONPOW2;
	}

	HRESULT hr;

	// �e�N�X�`���ǂݍ���
	hr = D3DXCreateTextureFromFileEx(lpD3DDev,
		pSrcFile,
		sizeFlag,			// Width
		sizeFlag,			// Height
		D3DX_DEFAULT,		// MipLevels
		0,					// Usage
		D3DFMT_UNKNOWN,		// Format
		D3DPOOL_MANAGED,	// Pool
		D3DX_DEFAULT,		// Filter
		D3DX_DEFAULT,		// MipFilter
		0,					// ColorKey	 �J���[�L�[ ARGB�Ŏw��B0�Ŗ����B 
		&m_Info,
		nullptr,
		&m_lpTex);

	if(hr != S_OK)
	{
		return false;
	}

	// �T�[�t�F�C�X�擾
	hr = m_lpTex->GetSurfaceLevel(0,&m_lpSur);
	if(hr != D3D_OK){
		return false;
	}

	// ���擾
	D3DSURFACE_DESC desc;
	m_lpTex->GetLevelDesc(0, &desc);

	// �摜��RECT���쐬���Ƃ�
	SetRect(&m_rc, 0, 0, m_Info.Width, m_Info.Height);

	return true;
}

bool CTexture::LoadTextureEx(
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
)
{
	Release();

	// �f�o�C�X�L��
	m_lpD3DDev = lpD3DDev;
	m_lpD3DDev->AddRef();	// �Q�ƃJ�E���^���P���₷

	// 
	m_strName = pSrcFile;

	HRESULT hr;

	// �e�N�X�`���ǂݍ���
	hr = D3DXCreateTextureFromFileEx(lpD3DDev,
								pSrcFile,
								Width,
								Height,
								MipLevels,
								Usage,
								Format,
								Pool,
								Filter,
								MipFilter,
								ColorKey,		//�J���[�L�[ ARGB�Ŏw��B0�Ŗ����B 
								&m_Info,
								pPalette,
								&m_lpTex);
	if(hr != D3D_OK){
		return false;
	}

	// �T�[�t�F�C�X�A�h���X�擾
	hr = m_lpTex->GetSurfaceLevel(0,&m_lpSur);
	if(hr != D3D_OK){
		return false;
	}

	// ���擾
	D3DSURFACE_DESC desc;
	m_lpTex->GetLevelDesc(0,&desc);

	// �摜��RECT���쐬���Ƃ�
	SetRect(&m_rc,0,0,m_Info.Width,m_Info.Height);


	return true;
}

bool CTexture::CreateTexture(
					LPDIRECT3DDEVICE9 lpD3DDev,
					UINT Width,
					UINT Height,
					UINT MipLevels,
					DWORD Usage,
					D3DFORMAT Format,
					D3DPOOL Pool
){
	Release();

	// �f�o�C�X�L��
	m_lpD3DDev = lpD3DDev;
	m_lpD3DDev->AddRef();	// �Q�ƃJ�E���^���P���₷

	// �ǂݍ���
	HRESULT hr;
	hr = D3DXCreateTexture(lpD3DDev,Width,Height,MipLevels,Usage,Format,Pool,&m_lpTex);
	if(hr != D3D_OK){
		return false;
	}

	// �T�[�t�F�C�X�A�h���X�擾
	hr = m_lpTex->GetSurfaceLevel(0,&m_lpSur);
	if(hr != D3D_OK){
		return false;
	}

	// �摜���쐬
	D3DSURFACE_DESC desc;
	m_lpSur->GetDesc(&desc);

	m_Info.Width = desc.Width;
	m_Info.Height = desc.Height;
	m_Info.MipLevels = m_lpTex->GetLevelCount();
	m_Info.Format = desc.Format;

	SetRect(&m_rc,0,0,m_Info.Width,m_Info.Height);

	// ���擾
	m_lpTex->GetLevelDesc(0, &desc);

	return true;
}

void CTexture::SetTexture(DWORD StageNo)
{
	m_lpD3DDev->SetTexture(StageNo, m_lpTex);
}

HRESULT CTexture::SetRenderTarget(DWORD Index)
{
	return m_lpD3DDev->SetRenderTarget(Index, m_lpSur);
}

void CTexture::Release()
{
	SAFE_RELEASE(m_lpD3DDev);	// �f�o�C�X�̎Q�ƃJ�E���^�����炷
	SAFE_RELEASE(m_lpSur);// �T�[�t�F�X���
	SAFE_RELEASE(m_lpTex);// �e�N�X�`���[���
	m_strName.clear();

	ZeroMemory(&m_Info,sizeof(m_Info));
	SetRect(&m_rc,0,0,0,0);
}
