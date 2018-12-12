#include"../StandardLibraryInclude.h"
#include"../DirectX/D3D.h"
#include"../Mouse/Mouse.h"
#include"Camera.h"

Camera::Camera(int _flg)
{
	//�����l�ݒ�
	angle = 0;
	D3DXMatrixTranslation(&mat, 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&targetmat, 0.0f, 0.0f, 0.0f);
	if (_flg == ORIGINAL) {
		pos = D3DXVECTOR3(0.0f, 1.0f, 0.01f);
		look = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
		head = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	}
	if (_flg == THREE_PERSON_PERSPECTiVE) {
		pos = D3DXVECTOR3(0.0f, 3.0f, -7.0f);
		look = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		head = D3DXVECTOR3(0.0f, 1.0f, .0f);
	}
}

Camera::~Camera()
{

}


void Camera::ComplianceUpdate()
{
	//�J�����̉�]
	angle = MOUSE.GetMoveY();							//�}�E�X�̉�]�����擾
	D3DXMatrixRotationY(&mat, D3DXToRadian(angle));		//�J�����̍s������E�ɉ�]������

	//���W�X�V
	D3DXVec3TransformCoord(&pos, &D3DXVECTOR3(0.0f, 3.0f, -5.0f), &mat);
	D3DXVec3TransformCoord(&look, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &targetmat);
	D3DXVec3TransformNormal(&head, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &targetmat);		//Normal�͈ړ��s����܂܂Ȃ�
	D3DXVec3Normalize(&head, &head);												//�x�N�g����n���Ƃ��͐��K�������Ă���n��

	pos.x += targetmat._41;															//���_����̃J�����̈ʒu
	pos.y += targetmat._42;
	pos.z += targetmat._43;

}


D3DXVECTOR3* Camera::GetPos()
{
	return &pos;
}

D3DXVECTOR3* Camera::GetLook()
{
	return &look;
}

D3DXVECTOR3* Camera::GetHead()
{
	return &head;
}

D3DXMATRIX* Camera::GetView()
{
	//�r���[�̍쐬
	D3DXMatrixLookAtLH(&view, &pos, &look, &head);
	return &view;
}

D3DXMATRIX* Camera::GetProj()
{
	// ���e�s��̐ݒ�
	D3DXMatrixPerspectiveFovLH(&proj, D3DXToRadian(60), (float)DX.SCRW / (float)DX.SCRH, 1.0f, 1000.0f);

	return &proj;
}

D3DXMATRIX Camera::GetRotMat()
{
	D3DXMATRIX rotation;
	D3DXMatrixIdentity(&rotation);
	D3DXMatrixRotationY(&rotation, D3DXToRadian(angle));		//���E�ɉ�]������
	return rotation;
}

D3DXMATRIX* Camera::GetMat()
{
	return &mat;
}

void Camera::SetMat(D3DXMATRIX* _mat)
{
	targetmat = *_mat;
}

