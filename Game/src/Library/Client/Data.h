#pragma once
class Data
{
public:
	Data();
	~Data();

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	SOCKET GetSocket();
	D3DXVECTOR3* GetPos();
	float GetX();
	float GetY();
	float GetZ();
	float GetAngle();
	int GetAnimation();
	void SetSocket(SOCKET _socket);
	void SetPos(D3DXVECTOR3 _pos);
	void SetX(float _x);
	void SetY(float _y);
	void SetZ(float _z);
	void SetAngle(float _angle);
	void SetAnimation(int _animation);

private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	SOCKET socket;
	D3DXVECTOR3 pos=D3DXVECTOR3(0,0,0);
	float angle=0.0f;
	int animation=0;
};

