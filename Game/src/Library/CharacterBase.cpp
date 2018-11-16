#include"Include.h"
#include"CharacterBase.h"


D3DXMATRIX CharacterBase::GetMat()
{
	return Bmat;
}

charastatus CharacterBase::Getstatus()
{
	return status;
}

void CharacterBase::SetMat(float _x, float _y, float _z)
{
	D3DXMATRIX tempmat;
	D3DXMatrixTranslation(&tempmat, _x, _y, _z);
	Bmat = tempmat;
}


//---------------------------------------------------------
//ïœêîmatÇÃílÇèëÇ´ä∑Ç¶ÇÈÇΩÇﬂÇÃä÷êî
//---------------------------------------------------------
void CharacterBase::Trans(float _x,float _y,float _z)
{
	D3DXMATRIX tempmat;
	D3DXMatrixTranslation(&tempmat, _x, _y, _z);
	Bmat = tempmat*Bmat;
}

void CharacterBase::RotationLeft(float _rotation)
{
	D3DXMATRIX tempmat;
	D3DXMatrixRotationY(&tempmat, D3DXToRadian(-_rotation));
	Bmat = tempmat*Bmat;
}

void CharacterBase::RotationRight(float _rotation)
{
	D3DXMATRIX tempmat;
	D3DXMatrixRotationY(&tempmat, D3DXToRadian(_rotation));
	Bmat = tempmat*Bmat;

}

void CharacterBase::Scale(float _x, float _y, float _z)
{
	D3DXMATRIX tempmat;
	D3DXMatrixScaling(&tempmat, _x, _y, _z);
	Bmat = tempmat*Bmat;

}

float CharacterBase::RayDecision()
{
	/*
	int nowx, nowz;
	nowx = (int)(mat._41 / 10);
	nowz = (int)(mat._43 / 10);
	D3DXVECTOR3 vpos[4];
	vpos[0] = D3DXVECTOR3(nowx*10.0f, Height[nowx][nowz], nowz*10.0f);
	vpos[1] = D3DXVECTOR3(nowx * 10.0f, Height[nowx][nowz + 1], (nowz + 1) * 10.0f);
	vpos[2] = D3DXVECTOR3((nowx + 1) * 10.0f, Height[nowx + 1][nowz + 1], (nowz + 1) * 10.0f);
	vpos[3] = D3DXVECTOR3((nowx + 1) * 10.0f, Height[nowx + 1][nowz], nowz * 10.0f);

	D3DXVECTOR3 playermat;

	float length;
	if (D3DXIntersectTri(&vpos[0], &vpos[1], &vpos[2], &mat, &D3DXVECTOR3(0, -1, 0), NULL, NULL, &Length) ||
		D3DXIntersectTri(&vpos[0], &vpos[2], &vpos[3], &mat, &D3DXVECTOR3(0, -1, 0), NULL, NULL, &Length)) {
		return lenght;
	}
	*/
	return 0;
}
