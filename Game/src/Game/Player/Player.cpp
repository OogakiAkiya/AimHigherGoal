#include"../../Include.h"
#include"Player.h"

Player::Player()
{
	//�C���X�^���X�̐���
	camera = new Camera();									//�J�����ݒ�
	data = new Data();

	// �X�L�����b�V���ǂݍ���
	SPtr<CSkinMesh> sm = std::make_shared<CSkinMesh>();
	sm->LoadXFile("images/Hackadoll_1/model.x");
	skinBone.SetSkinMesh(sm);								// ���b�V���f�[�^���Z�b�g���A����p�{�[�����\�z
	skinBone.InitAnimator(skinAnimation);					// ���̃{�[���p�̃A�j���[�^���Z�b�g�A�b�v
	skinAnimation.ChangeAnime("����", true);				// �A�j���[�^�ɃA�j���[�V������ݒ�
	skinChara.CreateMove(0, 0, 0);							//�ړ�
	skinChara.RotateY_Local(0);								//��]
	data->SetAnimation(WAIT);								//�A�j���[�V�����쐬

	//�V�F�[�_�[�ǂݍ���
	toonShader.Init();

	//���[�U�[�f�[�^���M
	data->SetId("test");
	CLIENT.SendUserInformation(data);
}

Player::~Player()
{
	//�������
	toonShader.Release();
	delete camera;
	delete data;
}

void Player::Update()
{
	//�X�V����
	State();												//�v���C���[�̏�Ԃ��`�F�b�N�����̏��������s����
	D3DXMATRIX mat=*CreateMat();
	camera->SetMat(mat);									//�J������Player�̍s���n��
	camera->ComplianceUpdate();								//�Ǐ]�J�������N��
	skinChara.CreateMove(data->GetPos());					//�v���C���[�̍��W�ݒ�
	skinChara.RotateY_Local(data->GetAngle());				//angle����]
	skinAnimation.Animation(1);								// �A�j���[�V������i�s
	skinBone.CalcBoneMatrix();								// �S�{�[���ATransMat���g�p��LocalMat���X�V����

}


void Player::Render2D()
{
}

void Player::Render3D()
{
	// �Œ�@�\����A�r���[�s��Ǝˉe�s����擾
	DEV->GetTransform(D3DTS_VIEW, &camera->GetView());
	DEV->GetTransform(D3DTS_PROJECTION, &camera->GetProj());
	// �V�F�[�_���̃O���[�o���萔�Ɏˉe�s����Z�b�g
	toonShader.SetTransformProj(&camera->GetProj());
	// �V�F�[�_���̃O���[�o���萔�Ƀr���[�s����Z�b�g
	toonShader.SetTransformView(&camera->GetView());

	// �r���[�s�񂩂�J�����s��ɕϊ�����

	// �Œ�@�\�̃��C�g�����擾���A�V�F�[�_���̃O���[�o���萔�Ƀ��C�g�����Z�b�g
	D3DLIGHT9 light = DX.GetLight();	// �Œ�@�\�̃��C�g�����擾
	toonShader.SetDirectionalLight(&light);

	// �V�F�[�_���̃O���[�o���萔�ɃJ�������W���Z�b�g
	toonShader.SetCamPos(&D3DXVECTOR3(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z));
	toonShader.SetTransformWorld(&skinChara);	// �V�F�[�_���̃O���[�o���萔�Ƀ��[���h�s����Z�b�g����
	toonShader.DrawMesh(&skinBone);			// �V�F�[�_�ŕ`��
	

}


void Player::State()
{
	//�_���[�W���󂯂鏈��(�f�o�b�N�p)
	if (GetAsyncKeyState('Q') & 0x8000&&data->GetAnimation()!=DAMAGE) {
		data->SetAnimation(DAMAGE);
		acceleration.z = -0.17;
	}

	//�e���菈��
	if (data->GetAnimation() != WAKEUP) {
		if (data->GetAnimation() != DAMAGE) {															//�_���[�W���󂯂Ă����炻�̑��̓�������Ȃ�
			if (data->GetAnimation() != ATTACK) {														//�v���C���[���U�����Ă���Ȃ炻�̑��̓�������Ȃ�
				if (jumpFlg == 0) data->SetAnimation(WAIT);												//�������삵�Ă��Ȃ��Ȃ�ҋ@���[�V�����ɐݒ�

				if (data->GetAnimation() != JUMPUP&&data->GetAnimation() != JUMPDOWN&&data->GetAnimation() != LANDING) {			//�v���C���[���W�����v���Ă��Ȃ�������ړ����[�V�������g�p����
					if (!GetAsyncKeyState('S') &&GetAsyncKeyState('W') & 0x8000) data->SetAnimation(WALK);						//�ړ���Ԃɐݒ�
					if (!GetAsyncKeyState('D') && GetAsyncKeyState('A') & 0x8000) data->SetAnimation(WALK);						//�ړ���Ԃɐݒ�
					if (!GetAsyncKeyState('W') &&GetAsyncKeyState('S') & 0x8000) data->SetAnimation(WALK);						//�ړ���Ԃɐݒ�
					if (!GetAsyncKeyState('A') && GetAsyncKeyState('D') & 0x8000) data->SetAnimation(WALK);						//�ړ���Ԃɐݒ�
				}

				if (GetAsyncKeyState(VK_SPACE) & 0x8000
					&& data->GetAnimation() != JUMPUP
					&& data->GetAnimation() != JUMPDOWN
					&& data->GetAnimation() != LANDING) {
					data->SetAnimation(JUMPUP);															//�W�����v��Ԃɐݒ�
				}
			}
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && jumpFlg == 0) data->SetAnimation(ATTACK);																//�W�����v���Ă��Ȃ���Ԃō��N���b�N�ōU��
		}
	}

	//�A�j���[�V�����ɂ�菈�������肷��
	switch (data->GetAnimation()) {
	case WAIT:							//�ҋ@���[�V����
		Wait();
		break;
	case WALK:							//�ړ����[�V����
		Move();
		break;
	case JUMPUP:						//�W�����v���[�V����(�㏸)
	case JUMPDOWN:						//�W�����v���[�V����(����)
	case LANDING:						//���n
		Jump();
		break;
	case ATTACK:						//�U�����[�V����
		Attack();
		break;
	case DAMAGE:						//�_���[�W
		Damage();
		break;
	case WAKEUP:						//�N���オ��
		WakeUp();
		break;

	}

	//����̏����̎��̂݃��[�v�A�j���[�V������ݒ�
	if (data->GetAnimation() == WAIT || data->GetAnimation() == WALK || data->GetAnimation() == RUN) {
		ChangeAnimation(data->GetAnimation(), true);
	}
	else {
		ChangeAnimation(data->GetAnimation(), false);

	}

	//�v���C���[�̍��W����ɑ��M
	CLIENT.SendPos(data);											//�T�[�o�[�ɍ��W�𑗐M

}

void Player::Attack()
{
	//�U������
	if (skinAnimation.GetAnimeNo() == ATTACK&&skinAnimation.GetAnimePos()== 20) {
		CLIENT.SendAttack(data);													//�U�����������Ă��邩�ǂ����̔��f�Ɠ������Ă����ꍇ�f�[�^��G�ɑ���
	}

	if (skinAnimation.IsAnimationEnd() == true) data->SetAnimation(WAIT);			//�U�����I�������ҋ@���[�V�����ɖ߂�
}

void Player::Jump()
{
	//�W�����v����
	if (jumpFlg == JUMPRESET) {
		jumpFlg = JUMPUPER;											//�W�����v�㏸��
		acceleration.y = 0.2f;										//�W�����v���鏉���ݒ�
		data->SetAnimation(JUMPUP);									//�㏸�W�����v
	}

	//���~���̏���
	if (acceleration.y < 0&&jumpFlg==1) {
		jumpFlg = JUMPDOWNER;										//�W�����v���~��
		data->SetAnimation(JUMPDOWN);								//���~
	}

	//���n����
	if (data->GetY() <= 0) {											//�L�����N�^�[��y���W��0�����������Ȃ�����
		if (jumpFlg == JUMPDOWNER) {								//�W�����v���~���̂ݒʂ�
			jumpFlg = JUMPSTANDING;									//3�͒��n���
			data->SetAnimation(LANDING);								//���n
		}
		data->SetY(0);
	}

	//�W�����v�����I��
	if (jumpFlg == JUMPSTANDING) {
		if (skinAnimation.IsAnimationEnd() == true)jumpFlg = JUMPRESET;		//�A�j���[�V�������I�������W�����v�I��
		return;
	}

	D3DXVECTOR3 vec(0, acceleration.y, 0);
	TransDataPos(&vec);												//���W�̈ړ�
	acceleration.y -= 0.008;										//�����x�����炷
	Move();															//�W�����v��WASD�������ꂽ���̈ړ�����
}

void Player::Wait()
{
	data->SetAnimation(WAIT);										//�ҋ@���[�V����
	acceleration = { 0.001f,0.0f,0.0f };							//�����x������
}

void Player::Move()
{
	//���������邩�𔻒�
	if (data->GetAnimation() == WALK&&skinAnimation.GetAnimeNo() != RUN)data->SetAnimation(WALK);
	if (data->GetAnimation() == WALK&&acceleration.z > (MOVESPEEDLIMIT - 0.03))data->SetAnimation(RUN);			//���̑��x�ɒB����ƃ_�b�V�����[�V����������

	D3DXVECTOR3 toVec(0.0f, 0.0f, 0.0f), tempVec;

	//�ړ�
	if (GetAsyncKeyState('W') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &camera->GetRotMat());
		toVec += tempVec;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		toVec += tempVec;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(-1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		toVec += tempVec;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, -1.0f), &camera->GetRotMat());
		toVec += tempVec;
	}

	Rotation(toVec);						//ToVec���ړ��Ɖ�]���s��
}

void Player::Damage()
{
	//�A�j���[�V�����ݒ�
	if (skinAnimation.GetAnimePos() >= 55 && data->GetAnimation() == DAMAGE) data->SetAnimation(WAKEUP);			//�_���[�W�A�j���[�V���������̈ʒu�܂ŗ����
																												//�N���オ�郂�[�V�����ɕύX����
	ChangeAnimation(data->GetAnimation(), false);
	
	//��ޏ���
	D3DXVECTOR3 vec=*GetVector(0,0,acceleration.z);
	TransDataPos(&vec);
	acceleration.z += 0.005f;											//�����x�����炷
	if (acceleration.z > 0)acceleration.z = 0.0f;

}

void Player::WakeUp()
{
	//�A�j���[�V�����ݒ�
	if (skinAnimation.GetAnimePos() == 26 && data->GetAnimation() == WAKEUP) data->SetAnimation(WAIT);

	//�O�i����
	D3DXVECTOR3 vec=*GetVector(0.0f, 0.0f, 0.025f);
	TransDataPos(&vec);
	ChangeAnimation(data->GetAnimation(), false);
}


void Player::Rotation(D3DXVECTOR3 _vec)
{
	//���[�J���ϐ�
	D3DXVECTOR3 toVec = _vec;
	D3DXMATRIX trans;																//�ړ��p�s��
	D3DXMATRIX rotation;															//��]�p�s��

	//������
	D3DXMatrixIdentity(&rotation);													//�P�ʍs��ɏ�����
	D3DXMatrixIdentity(&trans);														//�P�ʍs��ɏ�����

	//�ړ�
	D3DXMATRIX moveMat;
	D3DXVec3Normalize(&toVec, &toVec);												//���K��
	toVec *= acceleration.z;														//�ړ��ʂ�ݒ�
	D3DXMatrixTranslation(&moveMat, toVec.x, toVec.y, toVec.z);						//�s��̍쐬
	trans = moveMat*trans;															//�s��̍쐬
	acceleration.z += 0.0005;														//�ړ��p�����ϐ��̒l�𑫂�����
	if (acceleration.z > MOVESPEEDLIMIT) acceleration.z = MOVESPEEDLIMIT;			//����l�̐ݒ�
	if (jumpFlg == 0) {
		//��]
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data->GetAngle()));				//��]�p�s��ɉ�]�ʂ�����

		D3DXVECTOR3 nowVec;															//�@�������Ă����
		D3DXVec3TransformNormal(&nowVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &rotation);
		D3DXVec3Normalize(&toVec, &toVec);											//�A������������

		//�@�ƇA���g���ē��ςŊp�x���o��
		float rotationAngle;
		rotationAngle = D3DXVec3Dot(&nowVec, &toVec);
		rotationAngle = D3DXToDegree(acos(rotationAngle));
		if (rotationAngle >= 0.1f) {												//�@�ƇA�̊O�ς����߂Ă��̃x�N�g������������Ă���+���������Ă�����-����
			float nowAngle = data->GetAngle();
			D3DXVECTOR3 vCross;														//�@�ƇA�̊O��
			D3DXVec3Cross(&vCross, &nowVec, &toVec);								//�O��
			D3DXVec3Normalize(&vCross, &vCross);									//���K��

			if (rotationAngle >= 10.0f) { rotationAngle = 5.0f; }					//�����Ȃ�Ȃ��肷���Ȃ��悤�ɏ������ړ�����悤�ɂ���
			if (vCross.y >= 0.9) {													//�ق�1�ł���
				nowAngle += rotationAngle;
			}
			else if (vCross.y <= -0.9f) {											//�ق�-1�ł���
				nowAngle -= rotationAngle;
			}
			else {
				nowAngle += rotationAngle;											//�^���܂�^�t�x�N�g��������ꂽ�Ƃ��͑����Ă������Ă��悢
			}
			data->SetAngle(nowAngle);
		}
		//��]
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data->GetAngle()));
	}
	TransDataPos(trans);

	//SOCK.SendPos(&posdata);														//�ړ����̂݃T�[�o�[�ɍ��W�𑗐M

}

void Player::ChangeAnimation(int _animation,bool _roop,float _speed)
{
	if (skinAnimation.GetAnimeNo() == _animation) return;							//���݂̃A�j���[�V�����Ƃ������A�j���[�V�����������Ȃ�֐����甲����
		skinAnimation.ChangeAnimeSmooth(_animation, 0, 10, _roop,_speed);			//�A�j���[�V������_animation�ɕύX
}

void Player::TransDataPos(D3DXMATRIX _trans)
{
	D3DXVECTOR3 transpos(_trans._41, _trans._42, _trans._43);
	D3DXVECTOR3 pos(data->GetX(),data->GetY(),data->GetZ());
	pos += transpos;
	data->SetPos(pos);
}

void Player::TransDataPos(D3DXVECTOR3* _vec)
{
	D3DXVECTOR3 transpos(_vec->x, _vec->y, _vec->z);
	D3DXVECTOR3 pos(data->GetX(), data->GetY(), data->GetZ());
	pos += transpos;
	data->SetPos(pos);
}


D3DXMATRIX* Player::CreateMat()
{
	D3DXMATRIX returnmat;
	D3DXMatrixTranslation(&returnmat, data->GetX(), data->GetY(), data->GetZ());
	return &returnmat;
}

D3DXVECTOR3 * Player::GetVector(float _x,float _y,float _z)
{
	D3DXVECTOR3 vec;													//�v���C���[�̌��Ă��������������������ړ������l������
	D3DXMATRIX rotmat;													//�v���C���[�̉�]
	D3DXMatrixIdentity(&rotmat);
	D3DXMatrixRotationY(&rotmat, D3DXToRadian(data->GetAngle()));		//���E�ɉ�]������
	D3DXVec3TransformNormal(&vec, &D3DXVECTOR3(_x, _y, _z), &rotmat);

	return &vec;
}
