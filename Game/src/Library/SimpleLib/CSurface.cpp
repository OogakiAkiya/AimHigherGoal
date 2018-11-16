#include "SimpleLib.h"

using namespace SimpleLib;

BOOL CSurface::CreateDepthStencil(LPDIRECT3DDEVICE9 lpD3DDev, UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard)
{
	Release();

	// デバイス記憶
	m_lpD3DDev = lpD3DDev;
	m_lpD3DDev->AddRef();	// 参照カウンタを１増やす

	// 作成
	HRESULT hr = lpD3DDev->CreateDepthStencilSurface(Width, Height, Format,MultiSample, MultisampleQuality, Discard, &m_lpSur, nullptr);
	if(hr != D3D_OK)return FALSE;

	// 詳細情報取得
	m_lpSur->GetDesc(&m_Desc);

	return TRUE;
}

void CSurface::SetDepthStencil()
{
	m_lpD3DDev->SetDepthStencilSurface(m_lpSur);
}

void CSurface::Release()
{
	SAFE_RELEASE(m_lpSur);
	ZeroMemory(&m_Desc,sizeof(m_Desc));

	// デバイスの参照カウンタを減らす
	SAFE_RELEASE(m_lpD3DDev);
}
