#include"Include.h"
#include"Mouse.h"

Mouse* Mouse::s_Instance = nullptr;

Mouse::Mouse()
{
	mouseBasePt.x = 640;						//�X�N���[�����W�̉�ʓ��̒����������ʒu�Ƃ���
	mouseBasePt.y = 360;						//�X�N���[�����W�̉�ʓ��̒����������ʒu�Ƃ���

}

Mouse::~Mouse()
{
}

void Mouse::ThirdPersonPointUpdata()
{
	POINT mousePt;											//�|�C���^�̈ʒu��ۑ�����
	GetCursorPos(&mousePt);									//�|�C���^�̈ʒu�̎擾
	long MoveDis;											//�덷�͗�O�Ƃ݂Ȃ�����
	MoveDis = (mousePt.x - mouseBasePt.x);
	if (MoveDis > 5 || MoveDis < -5) {
		movey += (mousePt.x - mouseBasePt.x) / SENSITIVITY;			//���E�̉�]
	}

	
	MoveDis = (mousePt.y - mouseBasePt.y);
	if (abs(MoveDis) > 5) {
		movex += (mousePt.y - mouseBasePt.y) / SENSITIVITY;			//����2.0f�͊��x(�傫���قǓ݂��Ȃ�)
	}
	if (movex > 80)movex = 80;
	if (movex < -80)movex = -80;
	SetCursorPos(mouseBasePt.x, mouseBasePt.y);
}

void Mouse::FreeUpdate()
{
	GetCursorPos(&mouseBasePt);									//�|�C���^�̈ʒu�̎擾

}


POINT Mouse::GetMousePt()
{
	return mouseBasePt;
}

float Mouse::GetMoveX()
{
	return movex;
}

float Mouse::GetMoveY()
{
	return movey;
}

Mouse & Mouse::GetInstance()
{

	if (s_Instance == nullptr) { s_Instance = new Mouse(); }
	return *s_Instance;

}

void Mouse::DeleteInstance()
{
	if (s_Instance) {
		delete s_Instance;
		s_Instance = nullptr;
	}
}