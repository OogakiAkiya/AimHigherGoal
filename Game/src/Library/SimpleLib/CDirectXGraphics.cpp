#include "SimpleLib.h"

using namespace SimpleLib;

//================================================================================
//
// Direct3Dを初期化する。
//	引数：hWnd･･･Direct3Dで使用するウィンドウ
//	　　　Width･･･幅
//　　　　Height･･･高さ
//　　　　Format･･･色モード
//　　　　FullScreen･･･フルスクリーンフラグ
//　戻り値：成功･･･TRUE
//　　　　　失敗･･･FALSE
//
//================================================================================
bool CDirectXGraphics::Init(HWND hWnd, int Width, int Height, D3DFORMAT Format, bool FullScreen)
{
	//=======================================================
	// D3Dオブジェクト作成
	//=======================================================
	m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_lpD3D == nullptr) {
		return false;
	}

	//=======================================================
	// 現在の画面モードを取得
	//=======================================================
	D3DDISPLAYMODE d3ddm;
	if( FAILED( m_lpD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) ) return false;

	//=======================================================
	// 設定
	//=======================================================
	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
	m_d3dpp.BackBufferCount = 2;
	if (FullScreen){
		m_d3dpp.Windowed = FALSE;
		m_d3dpp.BackBufferWidth = Width;
		m_d3dpp.BackBufferHeight = Height;
		m_d3dpp.BackBufferFormat = Format;
	}else{
		m_d3dpp.Windowed = TRUE;
		m_d3dpp.BackBufferWidth = 0;
		m_d3dpp.BackBufferHeight = 0;
		m_d3dpp.BackBufferFormat = d3ddm.Format;
	}
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	// スワップ エフェクト
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// モニタの垂直同期を待たない
	m_d3dpp.hDeviceWindow = hWnd;
	// Z バッファの自動作成
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;	// 24bitZバッファ + 8bitステンシルバッファ

	//デバイスの作成 - T&L HAL
	// ※D3DCREATE_MULTITHREADED	…　マルチスレッドでのデバイス操作を許可
	// ※D3DCREATE_FPU_PRESERVE		…　floatの制度を下げずに維持する(高精度なfloatを使う)
	if(FAILED(m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,&m_d3dpp, &m_lpD3DDev)))
	{
		//失敗したのでHALで試行
		if(FAILED(m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,&m_d3dpp, &m_lpD3DDev)))
		{
			//失敗したのでREFで試行
			if(FAILED(m_lpD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_REF,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,&m_d3dpp, &m_lpD3DDev)))
			{
				//結局失敗
				MessageBox(hWnd, "Direct3Dの初期化に失敗", "Direct3D初期化" , MB_OK | MB_ICONSTOP);
				//終了する
				return false;
			}
		}
	}

	//=======================================================
	// Caps取得
	//=======================================================
	m_lpD3DDev->GetDeviceCaps(&m_Caps);
	
	//=======================================================
	// D3DXSPRITE作成
	//=======================================================
	D3DXCreateSprite(m_lpD3DDev,&m_lpSprite);

	//=======================================================
	// D3DXFONT作成
	//=======================================================
	D3DXCreateFont( m_lpD3DDev, 12, 0, 0, 1, FALSE, DEFAULT_CHARSET, 
						 OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
						 "ＭＳ Ｐゴシック", &m_lpFont );
	m_lpFont->GetDesc(&m_FontDesc);

	//=======================================================
	// バックバッファとZバッファ取得
	//=======================================================
	m_lpD3DDev->GetRenderTarget(0, &m_OrgRenderTarget);
	m_lpD3DDev->GetDepthStencilSurface(&m_OrgDepthStencil);

	return true;
}

void CDirectXGraphics::Release()
{
	SAFE_RELEASE(m_OrgRenderTarget);
	SAFE_RELEASE(m_OrgDepthStencil);

	// D3DXSPRITE解放
	SAFE_RELEASE(m_lpSprite);
	// D3DXFONT解放
	SAFE_RELEASE(m_lpFont);

	SAFE_RELEASE(m_lpD3DDev);
	SAFE_RELEASE(m_lpD3D);
}

//================================================================================
//
// 基本的かなぁと思うレンダーステートの設定
//  ※詳細※
//    ライトON、ライトの設定は適当、法線正規化、Z判定有効、Z書き込み有効、半透明モード、
//    右回りカリング、シングルテクスチャ、フォグOFF、フォグ設定は頂点線形フォグ設定・範囲は1～10000、
//    アルファブレンドON、ステンシルバッファON、グローシェーディング、テクスチャアドレッシングモードはWrap
//================================================================================
void CDirectXGraphics::SetState()
{
	// DirectGraphicsの詳細設定
	m_lpD3DDev->SetRenderState(D3DRS_ZENABLE,TRUE);					// Zバッファ有効
	m_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);			// Zバッファに書き込み有効
	m_lpD3DDev->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	
	m_lpD3DDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );		// アルファブレンド有効
	Blend_Alpha();													// 半透明モード

	m_lpD3DDev->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);		//頂点法線の自動正規化

	m_lpD3DDev->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);	// グローシェーディング
	m_lpD3DDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		// カリング CCW=右回り=表面のみ描画

	SeparateAlphaBlendEnable(TRUE);									// α個別計算

	// テクスチャのα値のブレンディング方法の設定
	// テクスチャα合成処理の方法
	m_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );    // 以下の引数の成分を乗算する
	m_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );    // テクスチャの色を使用
	m_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );    // 頂点の色を使用
	// 色を合成する方法
	m_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );    // 以下の引数の成分を乗算する
	m_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    // テクスチャの色を使用
	m_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );    // 頂点の色を使用

	// テクスチャの設定
	SetTextureFilter_Linear(0);

	m_lpD3DDev->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);	// ラップモード
	m_lpD3DDev->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);	// ラップモード

	// ミップマップ詳細レベルを設定する。
	SetMipmapBias(-1.0f);

	// ディレクショナルライト設定
	SetDirectionalLight(0,
						&D3DXVECTOR3(0.5f,-1,0.3f),
						&D3DXCOLOR(1.0f,1.0f,1.0f,1),
						&D3DXCOLOR(0.0f,0.0f,0.0f,1),
						&D3DXCOLOR(1.0f,1.0f,1.0f,1));

	// スペキュラ有効
	m_lpD3DDev->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	// フォグ(霧効果)
	m_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE); // FogはOFF
}

// LIGHT
D3DLIGHT9 CDirectXGraphics::GetLight(UINT LightNo)
{
	D3DLIGHT9 light;
	m_lpD3DDev->GetLight( LightNo, &light );	// LightNo番目にこのライトを設定
	return light;
}

void CDirectXGraphics::SetDirectionalLight(int LightNo,D3DXVECTOR3 *vDir,D3DXCOLOR *Dif,D3DXCOLOR *Amb,D3DXCOLOR *Spe)
{
	// ディレクショナルライト設定
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type    = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = *Dif;	// ディヒューズ
	light.Ambient = *Amb;	// アンビエント
	light.Specular = *Spe;	// スペキュラ
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, vDir);

	m_lpD3DDev->SetLight( LightNo, &light );	// LightNo番目にこのライトを設定
	m_lpD3DDev->LightEnable( LightNo, TRUE );	// LightNo番目のライトを有効
}

void CDirectXGraphics::DrawSprite(CTexture* tex, D3DCOLOR color, D3DXMATRIX* mat)
{
	if(tex == nullptr)return;

	if(mat)m_lpSprite->SetTransform(mat);
	m_lpSprite->Draw(tex->GetTex(), tex->GetRect(), nullptr, nullptr, color);
}

void CDirectXGraphics::DrawSprite(CTexture* tex, int x, int y, int w, int h, D3DCOLOR color)
{
	if(tex == nullptr)return;
	CMatrix m;
	m.CreateScale(	(float)w / tex->GetInfo()->Width,
					(float)h / tex->GetInfo()->Height,
					0
				);
	m.Move((float)x,(float)y,0);

	m_lpSprite->SetTransform(&m);
	m_lpSprite->Draw(tex->GetTex(), tex->GetRect(), nullptr, nullptr, color);
}

void CDirectXGraphics::DrawFont(const char* text,  D3DCOLOR color, D3DXMATRIX* mat)
{
	if(text == nullptr)return;

	if(mat)m_lpSprite->SetTransform(mat);
	RECT rc = {0,0,0,0};
	m_lpFont->DrawText(m_lpSprite, text, -1, &rc, DT_CALCRECT, color);
	m_lpFont->DrawText(m_lpSprite, text, -1, &rc, DT_LEFT, color);
}

void CDirectXGraphics::DrawQuad(float x,float y,float w,float h,float tuCnt,float tvCnt,D3DCOLOR color)
{
	// 座標変換済み座標とテクスチャ座標
	struct VERTEX{
		float x, y, z, rhw;  // 位置
		float tu,tv;	// テクスチャ
	};

	VERTEX v[4];
	v[0].x = x;		v[0].y = y;
	v[1].x = x+w;	v[1].y = y;
	v[2].x = x;		v[2].y = y+h;
	v[3].x = x+w;	v[3].y = y+h;

	v[0].tu = 0;
	v[0].tv = 0;
	v[1].tu = tuCnt;
	v[1].tv = 0;
	v[2].tu = 0;
	v[2].tv = tvCnt;
	v[3].tu = tuCnt;
	v[3].tv = tvCnt;

	// RHW, 頂点色の設定
	for (int i=0; i<4; i++) {
		v[i].z = 0;
		v[i].rhw = 1;
	}

	m_lpD3DDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

	m_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VERTEX));

}
void CDirectXGraphics::DrawQuad3D(float LSize,float RSize,float TSize,float BSize,float tuCnt,float tvCnt,D3DXCOLOR color)
{
	// 座標とテクスチャ座標
	struct VERTEX{
		float x, y, z;  // 位置
		float tu,tv;	// テクスチャ
	};

	VERTEX v[4];
	v[0].x = -LSize;	v[0].y = -BSize;	// 左下
	v[1].x = RSize;		v[1].y = -BSize;	// 右下
	v[2].x = -LSize;	v[2].y = TSize;		// 左上
	v[3].x = RSize;		v[3].y = TSize;		// 右上

	v[0].tu = tuCnt;
	v[0].tv = tvCnt;
	v[1].tu = 0;
	v[1].tv = tvCnt;
	v[2].tu = tuCnt;
	v[2].tv = 0;
	v[3].tu = 0;
	v[3].tv = 0;

	// RHW, 頂点色の設定
	for (int i=0; i<4; i++) {
		v[i].z = 0;
	}

	m_lpD3DDev->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	m_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VERTEX));

}
