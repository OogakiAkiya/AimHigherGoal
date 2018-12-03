#ifndef VisualCWrapper_h
#define VisualCWrapper_h

class VisualCWrapper{
public:
	bool Init(HINSTANCE _hInst, HINSTANCE _hPrev, LPSTR _lpszCmdParam, int _nCmdshow);
	//---------------------------------------------------------
	//���擾
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
	//�V���O���g��
	//---------------------------------------------------------
	static VisualCWrapper& GetInstance();
	static void DeleteInstance();


private:
	VisualCWrapper();
	~VisualCWrapper();

	//---------------------------------------------------------
	//�V���O���g��
	//---------------------------------------------------------
	static VisualCWrapper* s_Instance;
	VisualCWrapper & operator=(VisualCWrapper &);										//����̐錾
	VisualCWrapper(VisualCWrapper&);													//�R�s�[�R���X�g���N�^

	MSG msg;
	HWND hwnd;
	WNDCLASS wc;

	UINT uint;
	WPARAM wParam;
	LPARAM lParam;

};

#define WIN VisualCWrapper::GetInstance()
#endif