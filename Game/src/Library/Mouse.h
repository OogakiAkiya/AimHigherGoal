#ifndef Mouse_h
#define Mouse_h

class Mouse 
{
public:
	Mouse();
	~Mouse();
	void ThirdPersonPointUpdata();					//三人称視点の時に使用するとよいUpdate(マウスカーソルは中央に固定)
	void FreeUpdate();								//マウスカーソルが画面内を自由に移動できる

	//情報取得
	POINT GetMousePt();								//MousePt取得
	float GetMoveX();								//x方向の移動量を取得
	float GetMoveY();								//y方向の移動量を取得

	// シングルトン
	static Mouse & GetInstance();					//インスタンスの取得
	static void DeleteInstance();					//インスタンスの削除

private:
	//定数宣言
	static const int SENSITIVITY=2;	

	static Mouse* s_Instance;						//インスタンス
	float movey=0, movex=0;								//マウスのx・yの移動量
	POINT mouseBasePt;								//マウスの座標取得

};
#define MOUSE Mouse::GetInstance()

#endif