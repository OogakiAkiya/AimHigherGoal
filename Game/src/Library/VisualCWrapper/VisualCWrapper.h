#ifndef VisualCWrapper_h
#define VisualCWrapper_h

class VisualCWrapper{
public:
	bool Init(HINSTANCE _hInst, HINSTANCE _hPrev, LPSTR _lpszCmdParam, int _nCmdshow);
	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	MSG* GetMsg();
	HWND* GetHwnd();
	WNDCLASS* GetWndClass();
	UINT GetUintMsg();
	WPARAM GetwParam();
	LPARAM GetlParam();
	void SetUintMsg(UINT _msg);
	void SetwParam(WPARAM _wparam);
	void SetlParam(LPARAM _lParam);
	void Sethwnd(HWND _hwnd);
	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static VisualCWrapper& GetInstance();
	static void DeleteInstance();


private:
	VisualCWrapper();
	~VisualCWrapper();

	//---------------------------------------------------------
	//シングルトン
	//---------------------------------------------------------
	static VisualCWrapper* s_Instance;
	VisualCWrapper & operator=(VisualCWrapper &);										//代入の宣言
	VisualCWrapper(VisualCWrapper&);													//コピーコンストラクタ

	MSG msg;
	HWND hwnd;
	WNDCLASS wc;

	UINT uint;
	WPARAM wParam;
	LPARAM lParam;

};

#define WIN VisualCWrapper::GetInstance()
#endif