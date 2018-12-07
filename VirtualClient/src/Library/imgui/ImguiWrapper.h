#ifndef ImguiWrapper_h
#define ImguiWrapper_h

class ImguiWrapper {
public:
	ImguiWrapper(HWND _hwnd);
	~ImguiWrapper();
	void UpdataStart();
	void UpdataEnd();
	bool* GetApperFlg();
	void SetAppearFlg(bool _flg);
	void Draw();
private:
	bool appearFlg = true;
};

#endif