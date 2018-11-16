#include"../Include.h"
#include "Data.h"


Data::Data()
{
}


Data::~Data()
{
}

SOCKET Data::GetSocket()
{
	return socket;
}

float Data::GetX()
{
	return x;
}

float Data::GetY()
{
	return y;
}

float Data::GetZ()
{
	return z;
}

float Data::GetAngle()
{
	return angle;
}

int Data::GetAnimation()
{
	return animation;
}

void Data::SetSocket(SOCKET _socket)
{
	socket = _socket;
}

void Data::SetX(float _x)
{
	x = _x;
}

void Data::SetY(float _y)
{
	y = _y;
}

void Data::SetZ(float _z)
{
	z = _z;
}

void Data::SetAngle(float _angle)
{
	angle = _angle;
}

void Data::SetAnimation(int _animation)
{
	animation = _animation;
}

