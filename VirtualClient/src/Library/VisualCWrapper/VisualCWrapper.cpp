#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"../imgui/ImguiWrapper.h"
#include"VisualCWrapper.h"

VisualCWrapper* VisualCWrapper::s_Instance = nullptr;

LRESULT APIENTRY WndFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WIN.Sethwnd(hwnd);
	WIN.SetlParam(lParam);
	WIN.SetUintMsg(msg);
	WIN.SetwParam(wParam);
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}



bool VisualCWrapper::Init(HINSTANCE _hInst, HINSTANCE _hPrev,LPSTR _lpszCmdParam, int _nCmdshow)
{
	char szAppName[] = "Generic Game SDK Window";

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WndFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		szAppName,
		"Direct X",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		DX.SCRW, DX.SCRH,
		NULL, NULL, _hInst,
		NULL);

	if (!hwnd)return FALSE;

	ShowWindow(hwnd, _nCmdshow);
	UpdateWindow(hwnd);
	SetFocus(hwnd);
	return true;
}

MSG* VisualCWrapper::GetMsg()
{
	return &msg;
}

HWND* VisualCWrapper::GetHwnd()
{
	return &hwnd;
}

WNDCLASS* VisualCWrapper::GetWndClass()
{
	return &wc;
}

UINT VisualCWrapper::GetUintMsg()
{
	return uint;
}

WPARAM VisualCWrapper::GetwParam()
{
	return wParam;
}

LPARAM VisualCWrapper::GetlParam()
{
	return lParam;
}

void VisualCWrapper::SetUintMsg(UINT _msg)
{
	uint = _msg;
}

void VisualCWrapper::SetwParam(WPARAM _wparam)
{
	wParam = _wparam;
}

void VisualCWrapper::SetlParam(LPARAM _lParam)
{
	lParam = _lParam;
}

void VisualCWrapper::Sethwnd(HWND _hwnd)
{
	hwnd = _hwnd;
}

VisualCWrapper & VisualCWrapper::GetInstance()
{
	if (s_Instance == nullptr) { s_Instance = new VisualCWrapper(); }
	return *s_Instance;
}

void VisualCWrapper::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}

}

VisualCWrapper::VisualCWrapper()
{
}

VisualCWrapper::~VisualCWrapper()
{
}
