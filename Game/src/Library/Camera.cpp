#include"Include.h"
#include"Mouse.h"
#include"Camera.h"

Camera::Camera()
{
	angle = 0;
	D3DXMatrixTranslation(&mat, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&targetmat, 0.0f, 0.0f, 0.0f);
	pos = D3DXVECTOR3(0.0f, 3.0f, -7.0f);
	look = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	head = D3DXVECTOR3(0.0f, 1.0f, .0f);

}

Camera::~Camera()
{
}


void Camera::ComplianceUpdate()
{
	
	/*�J�����̉�]*/
	angle = MOUSE.GetMoveY();							//�}�E�X�̉�]�����擾
	D3DXMatrixRotationY(&mat, D3DXToRadian(angle));		//�J�����̍s������E�ɉ�]������

	//
	D3DXVec3TransformCoord(&pos, &D3DXVECTOR3(0.0f, 3.0f, -5.0f), &mat);
	D3DXVec3TransformCoord(&look, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &targetmat);
	D3DXVec3TransformNormal(&head, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &targetmat);			//Normal�͈ړ��s����܂܂Ȃ�
	D3DXVec3Normalize(&head, &head);												//�x�N�g����n���Ƃ��͐��K�������Ă���n��

	pos.x += targetmat._41;	//���_����̃J������
	pos.y += targetmat._42;
	pos.z += targetmat._43;


	
}


D3DXVECTOR3 Camera::GetPos()
{
	return pos;
}

D3DXVECTOR3 Camera::GetLook()
{
	return look;
}

D3DXVECTOR3 Camera::GetHead()
{
	return head;
}

D3DXMATRIX Camera::GetView()
{
	D3DXMATRIX mview;
	D3DXMatrixLookAtLH(&mview, &pos, &look, &head);
	return mview;
}

D3DXMATRIX Camera::GetProj()
{
	D3DXMATRIX mProj;
	// ���e�s��̐ݒ�
	D3DXMatrixPerspectiveFovLH(&mProj, D3DXToRadian(60), (float)DX.SCRW / (float)DX.SCRH, 1.0f, 1000.0f);

	return mProj;
}

D3DXMATRIX Camera::GetRotMat()
{
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&temp);
	D3DXMatrixRotationY(&temp, D3DXToRadian(angle));		//�J�����̍s������E�ɉ�]������
	return temp;
}

D3DXMATRIX Camera::GetMat()
{
	return mat;
}

void Camera::SetMat(D3DXMATRIX _mat)
{
	targetmat = _mat;
}

