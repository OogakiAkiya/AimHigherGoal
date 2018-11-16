//==========================================================================================================
// 
// Helper.h
//
// 制作者：鎌田
//
//==========================================================================================================

#ifndef Helper_h
#define Helper_h

//=================================================================
//
// あると便利なマクロや関数
//
//=================================================================


#pragma warning(disable:4996)
#include <string>

namespace SimpleLib{

// 度 -> ラジアン 変換
#define ToRadian(x) ((x)*0.017453f)
// ラジアン -> 度 変換
#define ToDegree(x) ((x)*57.3f)

// 安全なポインタの解放 ※本当はdefineでなく、テンプレート関数などにしたほうが良い
#define SAFE_FREE(p)		{if(p){free(p);p=nullptr;}}				// mallocしたものを安全にfreeするマクロ
#define SAFE_RELEASE(p)		{if(p){p->Release();p=nullptr;}}		// COMオブジェクト系を安全にReleaseするマクロ
#define SAFE_DELETE(p)		{if(p){delete p;p=nullptr;}}			// newされた変数を安全に解放するマクロ
#define SAFE_DELETE_ARRAY(p){if(p){delete[] p;p=nullptr;}}			// newされた配列を安全に解放するマクロ

// 色関係の短縮名用
#define XRGB(r,g,b) D3DCOLOR_XRGB(r,g,b)
#define ARGB(a,r,g,b) D3DCOLOR_ARGB(a,r,g,b)
#define RGBA(r,g,b,a) D3DCOLOR_RGBA(r,g,b,a)
#define COLORVALUE(a,r,g,b) D3DCOLOR_COLORVALUE(r,g,b,a)

//======================================================
// スマートポインタ短縮名用
//======================================================
#define SPtr std::shared_ptr
#define UPtr std::unique_ptr

//======================================================
// FPS計測
//======================================================
DWORD FpsProc();

//======================================================
// ログ出力
//======================================================
void AddLogFile(char *LogFileName,char *str, bool bData=false);

//=================================================================
// 可変引数メッセージボックス
//=================================================================
inline void DispMsg(HWND hWnd,char *format, ...)
{
	char str[1024];
	va_list argptr;

	va_start(argptr, format);
	vsprintf(str,format, argptr);
	va_end(argptr);

	MessageBox(hWnd,str,"Message",MB_OK);
}

//======================================================
// キー判定(1回押し)
//======================================================
bool KeyCheck(int keyCode);

//======================================================
// ファイル名と拡張子の間に、指定文字列を挿入した文字列を生成
//======================================================
inline std::string ConvertExtFileName(const std::string& FileName, const std::string& ext)
{
	std::string extName = FileName;
	int pos = extName.find_last_of(".");
	if (pos == -1)return "";
	std::string tmp = ".";
	tmp += ext;
	extName.insert(pos, tmp);
	return extName;
}


//======================================================
// 現在のレンダーターゲットを取得し、あとで戻す便利クラス
//======================================================
class RT_BackUpper {
public:
	// コンストラクタ
	//  現在のRTの情報を取得し、憶えておく
	RT_BackUpper(LPDIRECT3DDEVICE9 lpD3DDev, int rt_Index) {
		m_rt_Index = rt_Index;
		lpD3DDev->GetRenderTarget(rt_Index, &m_rt);	// RT取得
	}

	// デストラクタ
	//  憶えておいたRTをセットしなおす
	~RT_BackUpper() {
		if (m_rt) {
			LPDIRECT3DDEVICE9 lpD3DDev;
			m_rt->GetDevice(&lpD3DDev);

			lpD3DDev->SetRenderTarget(m_rt_Index, m_rt);

			SAFE_RELEASE(m_rt);
			SAFE_RELEASE(lpD3DDev);
		}
	}

	void Restore() {
	}

private:
	int					m_rt_Index = 0;
	LPDIRECT3DSURFACE9	m_rt = nullptr;
};

//======================================================
// 現在のZバッファを取得し、あとで戻す便利クラス
//======================================================
class ZBuffer_BackUpper {
public:
	// コンストラクタ
	//  現在のRTの情報を取得し、憶えておく
	ZBuffer_BackUpper(LPDIRECT3DDEVICE9 lpD3DDev) {
		lpD3DDev->GetDepthStencilSurface(&m_z);	// RT取得
	}

	// デストラクタ
	//  憶えておいたRTをセットしなおす
	~ZBuffer_BackUpper() {
		if (m_z) {
			LPDIRECT3DDEVICE9 lpD3DDev;
			m_z->GetDevice(&lpD3DDev);

			lpD3DDev->SetDepthStencilSurface(m_z);

			SAFE_RELEASE(m_z);
			SAFE_RELEASE(lpD3DDev);
		}
	}

private:
	LPDIRECT3DSURFACE9	m_z = nullptr;
};

}


#endif
