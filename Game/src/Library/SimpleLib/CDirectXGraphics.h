//==========================================================================================================
// 
// CDirectXGraphics.h
//
// 制作者：鎌田
//
//==========================================================================================================

#ifndef CDirectXGraphics_h
#define CDirectXGraphics_h

namespace SimpleLib{

//==========================================================================================================
//
// 簡易Direct3D操作クラス
//	Direct3D9に必要な変数や、よく行う操作を関数化しています。
//
//==========================================================================================================
class CDirectXGraphics{
public:
	//=============================================
	// 取得系
	//=============================================
	// D3Dオブジェクト取得
	LPDIRECT3D9				GetD3D(){return m_lpD3D;}
	// D3Dデバイス取得
	LPDIRECT3DDEVICE9		GetDev(){return m_lpD3DDev;}
	// D3DSprite取得
	LPD3DXSPRITE			GetSprite(){return m_lpSprite;}
	// D3DFont取得
	LPD3DXFONT				GetFont(){return m_lpFont;}
	// D3Dパラメータ取得
	D3DPRESENT_PARAMETERS*	GetD3DPP(){return &m_d3dpp;}
	// ライト情報取得
	D3DLIGHT9				GetLight(UINT LightNo);
	// デバイス情報取得
	D3DCAPS9*				GetCaps(){return &m_Caps;}

	// X解像度取得(画面の幅)
	int						GetRezoW() const {return m_d3dpp.BackBufferWidth;}
	// Y解像度取得(画面の高)
	int						GetRezoH() const {return m_d3dpp.BackBufferHeight;}

	//=============================================
	// Direct3D初期化
	//=============================================
	bool Init(HWND hWnd, int Width, int Height, D3DFORMAT Format, bool FullScreen);
	// Direct3Dの基本的なレンダーステート等の設定
	void SetState();

	//=============================================
	// Direct3D解放
	//=============================================
	void Release();

	//=============================================
	// レンダーターゲット・Zバッファ関係
	//=============================================
	// クリア
	void Clear(bool bRenderTarget, bool bZBuffer, bool bStencilBuffer, D3DCOLOR color = 0xFFFFFFFF, float z = 1.0f, DWORD stencil = 0){
		DWORD flags = 0;
		if(bRenderTarget)	flags |= D3DCLEAR_TARGET;
		if(bZBuffer)		flags |= D3DCLEAR_ZBUFFER;
		if(bStencilBuffer)	flags |= D3DCLEAR_STENCIL;

		m_lpD3DDev->Clear(0, nullptr, flags, color, z, stencil);
	}

	// デフォルトのバックバッファをセット。
	void ResetRenderTarget(){
		m_lpD3DDev->SetRenderTarget(0,m_OrgRenderTarget);
		// 残りの1～3は解除
		m_lpD3DDev->SetRenderTarget(1, nullptr);
		m_lpD3DDev->SetRenderTarget(2, nullptr);
		m_lpD3DDev->SetRenderTarget(3, nullptr);
	}

	// デフォルトのZバッファをセット。
	void ResetDepthStencil(){
		m_lpD3DDev->SetDepthStencilSurface(m_OrgDepthStencil);
	}

	// レンダーターゲット変更
	void SetRenderTarget(DWORD Idx, LPDIRECT3DTEXTURE9 texture){
		if(texture == nullptr){
			m_lpD3DDev->SetRenderTarget(Idx, nullptr);
		}
		else{
			LPDIRECT3DSURFACE9 rt;
			if (SUCCEEDED(texture->GetSurfaceLevel(0, &rt))) {
				m_lpD3DDev->SetRenderTarget(Idx, rt);
				rt->Release();
			}
		}
	}

	// Zバッファ変更
	void SetDepthStencil(LPDIRECT3DSURFACE9 depth_stencil){
		m_lpD3DDev->SetDepthStencilSurface(depth_stencil);
	}

	// 平行光源作成・設定
	void SetDirectionalLight(int LightNo, D3DXVECTOR3 *vDir, D3DXCOLOR *Dif, D3DXCOLOR *Amb, D3DXCOLOR *Spe);

	//============================================================
	// レンダーステート設定系
	//============================================================
	// ライティング有効無効
	void LightEnable(BOOL bl){
		m_lpD3DDev->SetRenderState(D3DRS_LIGHTING, bl);
	}

	// Zバッファ判定有効無効
	void ZEnable(BOOL bl){
		m_lpD3DDev->SetRenderState(D3DRS_ZENABLE, bl);
	}

	// Zバッファ書き込み有効無効
	void ZWriteEnable(BOOL bl){
		m_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, bl);
	}

	// カリングモード設定 通常はD3DCULL_CCW カリングしない場合はD3DCULL_NONE
	void CullMode(DWORD Mode){
		m_lpD3DDev->SetRenderState(D3DRS_CULLMODE, Mode);
	}

	// アルファブレンド有効
	void AlphaBlendEnable(BOOL bl){
		m_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, bl);
	}

	// 半透明描画設定
	void Blend_Alpha(){
		m_lpD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		m_lpD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_lpD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}

	// 加算合成描画設定
	void Blend_Add(){
		m_lpD3DDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		m_lpD3DDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_lpD3DDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	// レンダリングターゲット個別アルファ設定
	void SeparateAlphaBlendEnable(BOOL enable){
		m_lpD3DDev->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, enable );
		if(enable){
			m_lpD3DDev->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD );
			m_lpD3DDev->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE );
			m_lpD3DDev->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA );
		}
	}

	//========================================================================
	// サンプラ
	//========================================================================
	// テクスチャ補間を線形に
	void SetTextureFilter_Linear(DWORD SamplerStageIndex)
	{
		m_lpD3DDev->SetSamplerState(SamplerStageIndex,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		m_lpD3DDev->SetSamplerState(SamplerStageIndex,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		m_lpD3DDev->SetSamplerState(SamplerStageIndex,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	}

	// テクスチャ補間を無しに
	void SetTextureFilter_Point(DWORD SamplerStageIndex)
	{
		m_lpD3DDev->SetSamplerState(SamplerStageIndex,D3DSAMP_MINFILTER,D3DTEXF_POINT);
		m_lpD3DDev->SetSamplerState(SamplerStageIndex,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
		m_lpD3DDev->SetSamplerState(SamplerStageIndex,D3DSAMP_MIPFILTER,D3DTEXF_NONE);
	}

	// ミップマップ詳細度設定
	void SetMipmapBias(float fVal){
		m_lpD3DDev->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS,*((DWORD*)&fVal) );
	}

	// Wrapモード
	void SetAddressU_Wrap(DWORD SamplerStageIndex){
		m_lpD3DDev->SetSamplerState(SamplerStageIndex, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	}
	void SetAddressV_Wrap(DWORD SamplerStageIndex){
		m_lpD3DDev->SetSamplerState(SamplerStageIndex, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	}

	// Clampモード
	void SetAddressU_Clamp(DWORD SamplerStageIndex){
		m_lpD3DDev->SetSamplerState(SamplerStageIndex, D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
	}
	void SetAddressV_Clamp(DWORD SamplerStageIndex){
		m_lpD3DDev->SetSamplerState(SamplerStageIndex, D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
	}

	//========================================================================
	// D3DXSprite
	//========================================================================
	// スプライト描画開始
	BOOL BeginSprite(DWORD Flags=D3DXSPRITE_ALPHABLEND){
		HRESULT hr = m_lpSprite->Begin(Flags);
		if(hr == D3D_OK)return TRUE;

		return FALSE;
	}

	// スプライト描画終了
	void EndSprite(){
		m_lpSprite->End();
	}

	// スプライト描画 ※BeginSprite()～EndSprite()の間で書くこと
	void DrawSprite(CTexture* tex, D3DCOLOR color, D3DXMATRIX* mat);
	void DrawSprite(CTexture* tex, int x, int y, int w, int h, D3DCOLOR color);	// 座標、幅、高さ指定Version
	void DrawFont(const char* text,  D3DCOLOR color, D3DXMATRIX* mat);

	//========================================================================
	// 2D描画の描画設定用
	//========================================================================
	void Begin2DState(){
		LightEnable(FALSE);
		ZEnable(FALSE);
		ZWriteEnable(FALSE);
		SetAddressU_Clamp(0);
		SetAddressV_Clamp(0);
	}
	void End2DState(){
		LightEnable(TRUE);
		ZEnable(TRUE);
		ZWriteEnable(TRUE);
		SetAddressU_Wrap(0);
		SetAddressV_Wrap(0);
	}

	//========================================================================
	// 簡易描画用
	//========================================================================
	// 2Dの四角形を描画
	void DrawQuad(float x, float y, float w, float h, float tuCnt = 1, float tvCnt = 1, D3DCOLOR color = 0xFFFFFFFF);

	// 3Dの四角形を描画
	void DrawQuad3D(float LSize, float RSize, float TSize, float BSize, float tuCnt = 1, float tvCnt = 1, D3DXCOLOR color = 0xFFFFFFFF);


private:

	IDirect3D9*					m_lpD3D = nullptr;				// D3Dオブジェクト
	IDirect3DDevice9*			m_lpD3DDev = nullptr;			// D3Dデバイス
	D3DPRESENT_PARAMETERS		m_d3dpp;						// 詳細設定
	D3DCAPS9					m_Caps;							// デバイスの情報

	LPD3DXSPRITE				m_lpSprite = nullptr;			// D3DXSPRITE
	LPD3DXFONT					m_lpFont = nullptr;				// D3DXFONT
	D3DXFONT_DESC				m_FontDesc;						// ↑のフォントの情報

	LPDIRECT3DSURFACE9			m_OrgRenderTarget = nullptr;	// デフォルトのバックバッファ
	LPDIRECT3DSURFACE9			m_OrgDepthStencil = nullptr;	// デフォルトの深度(Z)バッファ

// シングルトン用
public:

	CDirectXGraphics(){
	}

	static CDirectXGraphics &getInstance(){
		static CDirectXGraphics Instance;
		return Instance;
	}

};

// 簡単にアクセスできるようにするためのマクロ
#define cdg CDirectXGraphics::getInstance()


}
#endif
