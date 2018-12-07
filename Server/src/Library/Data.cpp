#include"../Include.h"
#include "Data.h"


Data::Data()
{
}


Data::~Data()
{
	id = nullptr;
}

SOCKET Data::GetSocket()
{
	return socket;
}

std::shared_ptr<std::string> Data::GetId()
{
	return id;
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

void Data::SetId(std::shared_ptr<std::string> _id)
{
	id = _id;
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

