//==========================================================================================================
// 
// CTexture.h
//
// 制作者：鎌田
//
//==========================================================================================================

#ifndef CTexture_h
#define CTexture_h

namespace SimpleLib{

//==============================================================================================
// テクスチャクラス
//	テクスチャの読み込み、RTテクスチャの作成・セットなどが可能
//　解放は自動で行われる
//==============================================================================================
class CTexture{
public:
	//===============================================================================================
	// 情報取得
	//===============================================================================================
	// テクスチャ取得
	LPDIRECT3DTEXTURE9		GetTex() const {return m_lpTex;}
	// サーフェイス取得
	LPDIRECT3DSURFACE9		GetSur() const {return m_lpSur;}
	// RECT取得
	const RECT*				GetRect() const {return &m_rc;}
	// ファイル名
	std::string				GetName() const {return m_strName;}
	// 情報
	const D3DXIMAGE_INFO*	GetInfo() const {return &m_Info;}

	//===============================================================================================
	// 読み込み・作成
	//===============================================================================================

	// 簡易版ファイルからテクスチャ読み込み
	// [引数] nonPow2 … 読み込んだテクスチャを2のべき乗に拡縮しないようにする
	bool LoadTexture(LPDIRECT3DDEVICE9 lpD3DDev, LPCTSTR pSrcFile, bool nonPow2 = false);
	
	// ファイルからテクスチャ読み込み(拡張)
	// 引数はD3DXCreateTextureFromFileEx()とほぼ同じ
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

	// テクスチャ作成
	bool CreateTexture(
						LPDIRECT3DDEVICE9 lpD3DDev,
						UINT Width,
						UINT Height,
						UINT MipLevels,
						DWORD Usage,
						D3DFORMAT Format,
						D3DPOOL Pool
						);

	// レンダーターゲットテクスチャとして作成
	bool CreateRenderTarget(LPDIRECT3DDEVICE9 lpD3DDev, UINT Width,	UINT Height, D3DFORMAT Format = D3DFMT_A8R8G8B8){
		return CreateTexture(lpD3DDev, Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT);
	}

	//===============================================================================================
	// テクスチャをデバイスに設定
	//===============================================================================================
	void SetTexture(DWORD StageNo=0);

	//===============================================================================================
	// 解放
	//===============================================================================================
	void Release();
	
	//===============================================================================================
	// レンダーターゲットに設定
	//===============================================================================================
	HRESULT SetRenderTarget(DWORD Index=0);

	//===============================================================================================
	// 直接加工
	//===============================================================================================
	template<class T>
	bool Fill(T fill, DWORD LockFlag=0);	// 塗りつぶし

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

	LPDIRECT3DTEXTURE9	m_lpTex = nullptr;		// テクスチャ
	LPDIRECT3DSURFACE9	m_lpSur = nullptr;		// サーフェイス
	D3DXIMAGE_INFO		m_Info;					// 情報
	RECT				m_rc;					// RECT
	std::string			m_strName;				// ファイル名

	LPDIRECT3DDEVICE9	m_lpD3DDev = nullptr;		// Direct3D Device記憶用

private:
	// コピー禁止用
	CTexture(const CTexture& src){}
	void operator=(const CTexture& src){}
};



// 
template<class T>
bool CTexture::Fill(T fill,DWORD LockFlag)
{
	D3DLOCKED_RECT LockedRect;
	// ロックして、ピクセルの先頭アドレス取得
	if(SUCCEEDED(m_lpTex->LockRect(0, &LockedRect, nullptr, LockFlag))){
		//==================================
		//ここで画像データを転送
		//==================================
		T* p1 = (T*)LockedRect.pBits;
		T pitch = LockedRect.Pitch / sizeof(T);
		for ( UINT y = 0; y < m_Info.Height ; y++ )
		{
			for ( UINT x = 0; x < m_Info.Width; x++ )
			{
				p1[x] = fill;
			}
			p1 += pitch;    //次の行
		}
	
		m_lpTex->UnlockRect(0);
		return true;
	}
	return false;
}

}

#endif
