#ifndef Data_h
#define Data_h

class Data
{
public:
	Data();
	~Data();

	//---------------------------------------------------------
	//情報取得
	//---------------------------------------------------------
	SOCKET GetSocket();
	float GetX();
	float GetY();
	float GetZ();
	float GetAngle();
	int GetAnimation();

	//---------------------------------------------------------
	//情報設定
	//---------------------------------------------------------
	void SetSocket(SOCKET _socket);
	void SetX(float _x);
	void SetY(float _y);
	void SetZ(float _z);
	void SetAngle(float _angle);
	void SetAnimation(int _animation);

private:
	//---------------------------------------------------------
	//変数
	//---------------------------------------------------------
	SOCKET socket;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float angle = 0.0f;
	int animation = 0;
};

#endif