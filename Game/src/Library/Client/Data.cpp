#include"../StandardLibraryInclude.h"
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

D3DXVECTOR3* Data::GetPos()
{
	return &pos;
}

std::string * Data::GetId()
{
	return &id;
}

float Data::GetX()
{
	return pos.x;
}

float Data::GetY()
{
	return pos.y;
}

float Data::GetZ()
{
	return pos.z;
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

void Data::SetId(std::string _id)
{
	id = _id;
}

void Data::SetPos(D3DXVECTOR3 _pos)
{
	pos = _pos;
}

void Data::SetX(float _x)
{
	pos.x = _x;
}

void Data::SetY(float _y)
{
	pos.y = _y;
}

void Data::SetZ(float _z)
{
	pos.z = _z;
}

void Data::SetAngle(float _angle)
{
	angle = _angle;
}

void Data::SetAnimation(int _animation)
{
	animation = _animation;
}

