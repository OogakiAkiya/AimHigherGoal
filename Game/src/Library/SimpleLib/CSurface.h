//==========================================================================================================
// 
// CSurface.h
//
// 制作者：鎌田
//
//==========================================================================================================

#ifndef CSurface_h
#define CSurface_h

namespace SimpleLib{

//==================================
// サーフェイスクラス
//	特にZバッファの作成・セットの用途で使用
//==================================
class CSurface{
public:
	//==================================================================================
	// 取得
	//==================================================================================
	// サーフェイス取得
	LPDIRECT3DSURFACE9		GetSur(){return m_lpSur;}
	// 情報取得
	D3DSURFACE_DESC*		GetDesc(){return &m_Desc;}

	//==================================================================================
	// 作成
	//==================================================================================
	// Zバッファとして作成
	BOOL CreateDepthStencil(LPDIRECT3DDEVICE9 lpD3DDev, UINT Width, UINT Height, D3DFORMAT Format = D3DFMT_D24S8, D3DMULTISAMPLE_TYPE MultiSample = D3DMULTISAMPLE_NONE, DWORD MultisampleQuality = 0, BOOL Discard = FALSE);

	//==================================================================================
	// 深度バッファとしてセットする
	//==================================================================================
	void SetDepthStencil();

	//==================================================================================
	// 解放
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

	// Direct3D Device記憶用
	LPDIRECT3DDEVICE9		m_lpD3DDev = nullptr;

private:
	// コピー禁止用
	CSurface(const CSurface& src){}
	void operator=(const CSurface& src){}
};

}
#endif