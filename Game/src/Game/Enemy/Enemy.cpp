#include"../../Include.h"
#include"Enemy.h"

Enemy::Enemy(int _id)
{
	//camera = new Camera();									//�J�����ݒ�
	enemyid = _id;
	switch (enemyid) {
	case 0:
		D3DXMatrixTranslation(&initmat, 5.0f, 0.0f, 0.0f);			//�������W��ݒ�
		break;
	case 1:
		D3DXMatrixTranslation(&initmat, 0.0f, 0.0f, 5.0f);			//�������W��ݒ�
		break;
	case 2:
		D3DXMatrixTranslation(&initmat, -5.0f, 0.0f, 0.0f);			//�������W��ݒ�
		break;
	}
	TransDataPos(initmat);

	// �X�L�����b�V���ǂݍ���
	SPtr<CSkinMesh> sm = std::make_shared<CSkinMesh>();
	sm->LoadXFile("images/Hackadoll_1/model.x");

	m_bcChara.SetSkinMesh(sm);								// ���b�V���f�[�^���Z�b�g���A����p�{�[�����\�z
	m_bcChara.InitAnimator(m_anim);							// ���̃{�[���p�̃A�j���[�^���Z�b�g�A�b�v
	m_anim.ChangeAnime("����", true);						// �A�j���[�^�ɃA�j���[�V������ݒ�


	m_mChara.CreateMove(data.GetPos());							//�ړ�
	data.SetAnimation(WAIT);

	//m_toonshader.Init();

}

Enemy::~Enemy()
{
	//m_toonshader.Release();
	//delete camera;
}

void Enemy::Update()
{
	//State();												//�v���C���[�̏�Ԃ��`�F�b�N�����̏��������s����
	//camera->SetMat(mat);									//�J������Player�̍s���n��
	//camera->ComplianceUpdate();								//�Ǐ]�J�������N��

	if (standflg == true) {
		if (m_anim.GetAnimePos()== 26&&data.GetAnimation()==WAKEUP) {
			standflg = false;
			data.SetAnimation(WAIT);
			if (CLIENT.DataEmpty() == false) {
				CLIENT.Lock();
				CLIENT.ClearData();										//�擾�f�[�^�S�폜
				CLIENT.Unlock();
			}
			return;
		}
		ChangeAnimation(data.GetAnimation(), false);
		m_mChara.CreateMove(data.GetPos());			//�v���C���[�̍��W�ݒ�
		m_mChara.RotateY_Local(data.GetAngle());							//angle����]
		m_anim.Animation(1);									// �A�j���[�V������i�s
		m_bcChara.CalcBoneMatrix();								// �S�{�[���ATransMat���g�p��LocalMat���X�V����
		return;
	}


	if (data.GetAnimation()==DAMAGE) {								//�_���[�W�A�j���[�V�����̐���
		Damage();
		return;
	}

	//�����Ă����������G�l�~�[�̈ړ�
	if (CLIENT.DataEmpty()==false) {
		Data recvdata;												//�G�l�~�[�̃f�[�^
		D3DXVECTOR3 trans;
		//D3DXMatrixIdentity(&trans);

		CLIENT.Lock();
		recvdata = CLIENT.GetData();									//�f�[�^�擾
		CLIENT.Unlock();
		if (recvdata.GetAnimation() == DAMAGE) {					//�����Ă����f�[�^���_���[�W�������ꍇ
			data.SetAnimation(recvdata.GetAnimation());
			acceleration.z = 0.1;
		}
		else {
			trans.x = -recvdata.GetX();							//�}�C�i�X�]��
			trans.y = recvdata.GetY();
			trans.z = -recvdata.GetZ();							//�}�C�i�X�]��
			data.SetAngle(recvdata.GetAngle()+180);						//180�x�]��
			data.SetAnimation(recvdata.GetAnimation());

			data.SetPos(trans);									//�󂯎�������W���̈ړ�
			TransDataPos(initmat);							//�G�l�~�[�̏������W�������ړ�������
		}

		if (data.GetAnimation() == WAIT || data.GetAnimation() == WALK || data.GetAnimation() == RUN) {
			ChangeAnimation(data.GetAnimation(), true);
		}
		else {
			ChangeAnimation(data.GetAnimation(), false);
		}
	}

	//�N���C�A���g�N���X�Ƀf�[�^�Z�b�g
	CLIENT.SetEnemyData(enemyid, &data);

	m_mChara.CreateMove(data.GetPos());			//�v���C���[�̍��W�ݒ�
	m_mChara.RotateY_Local(data.GetAngle());							//angle����]
	m_anim.Animation(1);									// �A�j���[�V������i�s
	m_bcChara.CalcBoneMatrix();								// �S�{�[���ATransMat���g�p��LocalMat���X�V����

}


void Enemy::Render2D()
{
}

void Enemy::Render3D()
{
	
	// �L����(�Œ�@�\)
	DEV->SetTransform(D3DTS_WORLD, &m_mChara);				//�`�悷����W�ݒ�
	m_bcChara.Draw();										//�`��
	/*
	// �Œ�@�\����A�r���[�s��Ǝˉe�s����擾
	DEV->GetTransform(D3DTS_VIEW, &camera->GetView());
	DEV->GetTransform(D3DTS_PROJECTION, &camera->GetProj());
	// �V�F�[�_���̃O���[�o���萔�Ɏˉe�s����Z�b�g
	m_toonshader.SetTransformProj(&camera->GetProj());
	// �V�F�[�_���̃O���[�o���萔�Ƀr���[�s����Z�b�g
	m_toonshader.SetTransformView(&camera->GetView());

	// �r���[�s�񂩂�J�����s��ɕϊ�����

	// �Œ�@�\�̃��C�g�����擾���A�V�F�[�_���̃O���[�o���萔�Ƀ��C�g�����Z�b�g
	D3DLIGHT9 light = DX.GetLight();	// �Œ�@�\�̃��C�g�����擾
	m_toonshader.SetDirectionalLight(&light);

	// �V�F�[�_���̃O���[�o���萔�ɃJ�������W���Z�b�g
	m_toonshader.SetCamPos(&D3DXVECTOR3(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z));
	m_toonshader.SetTransformWorld(&m_mChara);	// �V�F�[�_���̃O���[�o���萔�Ƀ��[���h�s����Z�b�g����
	m_toonshader.DrawMesh(&m_bcChara);			// �V�F�[�_�ŕ`��
	*/

}

Camera* Enemy::GetCamera()
{
	//return camera;
	return nullptr;
}

void Enemy::State()
{

	if (data.GetAnimation() != ATTACK) {															//�v���C���[���U�����Ă���Ȃ炻�̑��̓�������Ȃ�
		if (jumpflg == 0) data.SetAnimation(WAIT);												//�������삵�Ă��Ȃ��Ȃ�ҋ@���[�V�����ɐݒ�

		if (data.GetAnimation() != JUMPUP&&data.GetAnimation() != JUMPDOWN&&data.GetAnimation() != LANDING) {			//�v���C���[���W�����v���Ă��Ȃ�������ړ����[�V�������g�p����
			if (GetAsyncKeyState('W') & 0x8000) data.SetAnimation(WALK);						//�ړ���Ԃɐݒ�
			if (GetAsyncKeyState('A') & 0x8000) data.SetAnimation(WALK);						//�ړ���Ԃɐݒ�
			if (GetAsyncKeyState('S') & 0x8000) data.SetAnimation(WALK);						//�ړ���Ԃɐݒ�
			if (GetAsyncKeyState('D') & 0x8000) data.SetAnimation(WALK);						//�ړ���Ԃɐݒ�
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000
			&& data.GetAnimation() != JUMPUP
			&& data.GetAnimation() != JUMPDOWN
			&& data.GetAnimation() != LANDING) {
			data.SetAnimation(JUMPUP);															//�W�����v��Ԃɐݒ�
		}
	}
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && jumpflg == 0) data.SetAnimation(ATTACK);																//�W�����v���Ă��Ȃ���Ԃō��N���b�N�ōU��

	switch (data.GetAnimation()) {
	case WAIT:							//�ҋ@���[�V����
		Wait();
		break;
	case WALK:							//�ړ����[�V����
		Move();
		break;
	case JUMPUP:						//�W�����v���[�V����
	case JUMPDOWN:
	case LANDING:
		Jump();
		break;
	case ATTACK:						//�U�����[�V����
		Attack();
		break;
	}

	if (data.GetAnimation() == WAIT || data.GetAnimation() == WALK || data.GetAnimation() == RUN) {
		ChangeAnimation(data.GetAnimation(), true);
	}
	else {
		ChangeAnimation(data.GetAnimation(), false);

	}


}

void Enemy::Attack()
{
	data.SetAnimation(ATTACK);												//�U�����[�V�����̐ݒ�

	if (m_anim.GetAnimeNo() == ATTACK&&m_anim.GetAnimePos() == 20) {
		CLIENT.SendAttack(&data);														//�U�����������Ă��邩�ǂ����̔��f�Ɠ������Ă����ꍇ�f�[�^��G�ɑ���
	}

	if (m_anim.IsAnimationEnd() == true) data.SetAnimation(WAIT);			//�U�����I�������ҋ@���[�V�����ɖ߂�
}

void Enemy::Jump()
{
	if (jumpflg == 0) {
		jumpflg = 1;												//1�̓W�����v�㏸��
		acceleration.y = 0.2f;										//�W�����v���鏉���ݒ�
		data.SetAnimation(JUMPUP);											//�㏸�W�����v
	}

	if (acceleration.y < 0 && jumpflg == 1) {
		jumpflg = 2;												//2�̓W�����v���~��
		data.SetAnimation(JUMPDOWN);										//���~
	}

	if (data.GetY() <= 0) {												//�L�����N�^�[��y���W��0�����������Ȃ�����
		if (jumpflg == 2) {											//�W�����v���~���̂ݒʂ�
			jumpflg = 3;											//3�͒��n���
			data.SetAnimation(LANDING);									//���n
		}
		data.SetY(0);
	}
	if (jumpflg == 3) {
		if (m_anim.IsAnimationEnd() == true)jumpflg = 0;			//�A�j���[�V�������I�������W�����v�I��
		return;
	}


	D3DXMATRIX translation;												//�ړ��ʕ��̍��W������s��
	D3DXMatrixTranslation(&translation, 0, acceleration.y, 0);			//y�������̈ړ�������
	TransDataPos(translation);											//���W�̈ړ�
	acceleration.y -= 0.008;											//�����x�����炷
	Move();																//�W�����v��WASD�������ꂽ���̈ړ�����
}

void Enemy::Wait()
{
	data.SetAnimation(WAIT);																//�ҋ@���[�V����
	acceleration = { 0.001f,0.0f,0.0f };											//�����x������
}

void Enemy::Move()
{
	if (data.GetAnimation() == WALK&&m_anim.GetAnimeNo() != RUN)data.SetAnimation(WALK);
	//if (data.GetAnimation() == WALK&&acceleration.z > (MOVESPEEDLIMIT - 0.03))data.SetAnimation(RUN);			//���̑��x�ɒB����ƃA�j���[�V����������

	D3DXVECTOR3 ToVec(0.0f, 0.0f, 0.0f), tempVec;
	//�ړ�
	if (GetAsyncKeyState('W') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(-1.0f, 0.0f, 0.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		//D3DXVec3TransformNormal(&tempVec, &D3DXVECTOR3(0.0f, 0.0f, -1.0f), &camera->GetRotMat());
		ToVec += tempVec;
	}

	Rotation(ToVec);						//ToVec���ړ��Ɖ�]���s��
}


void Enemy::Rotation(D3DXVECTOR3 _vec)
{
	/*���[�J���ϐ�*/
	D3DXVECTOR3 tovec = _vec;
	D3DXMATRIX trans;																//�ړ��p�s��
	D3DXMATRIX rotation;															//��]�p�s��

																					/*������*/
	D3DXMatrixIdentity(&rotation);													//�P�ʍs��ɏ�����
	D3DXMatrixIdentity(&trans);														//�P�ʍs��ɏ�����

																					/*�ړ�*/
	D3DXMATRIX move_mat;
	D3DXVec3Normalize(&tovec, &tovec);												//���K��
	tovec *= acceleration.z;														//�ړ��ʂ�ݒ�
	D3DXMatrixTranslation(&move_mat, tovec.x, tovec.y, tovec.z);						//�s��̍쐬
	trans = move_mat*trans;															//�s��̍쐬
	acceleration.z += 0.0005;														//�ړ��p�����ϐ��̒l�𑫂�����
	//if (acceleration.z > MOVESPEEDLIMIT) acceleration.z = MOVESPEEDLIMIT;			//����l�̐ݒ�
	if (jumpflg == 0) {
		/*��]*/
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data.GetAngle()));							//��]�p�s��ɉ�]�ʂ�����

		D3DXVECTOR3 nowvec;																//�@�������Ă����
		D3DXVec3TransformNormal(&nowvec, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &rotation);
		D3DXVec3Normalize(&tovec, &tovec);												//�A������������

																						//�@�ƇA���g���ē��ςŊp�x���o��
		float rotation_angle;
		rotation_angle = D3DXVec3Dot(&nowvec, &tovec);
		rotation_angle = D3DXToDegree(acos(rotation_angle));
		if (rotation_angle >= 0.1f) {													//�@�ƇA�̊O�ς����߂Ă��̃x�N�g������������Ă���+���������Ă�����-����
			float nowangle = data.GetAngle();
			D3DXVECTOR3 vCross;															//�@�ƇA�̊O��
			D3DXVec3Cross(&vCross, &nowvec, &tovec);									//�O��
			D3DXVec3Normalize(&vCross, &vCross);										//���K��

			if (rotation_angle >= 10.0f) { rotation_angle = 5.0f; }						//�����Ȃ�Ȃ��肷���Ȃ��悤�ɏ������ړ�����悤�ɂ���
			if (vCross.y >= 0.9) {														//�ق�1�ł���
				nowangle += rotation_angle;
			}
			else if (vCross.y <= -0.9f) {												//�ق�-1�ł���
				nowangle -= rotation_angle;
			}
			else {
				nowangle += rotation_angle;												//�^���܂�^�t�x�N�g��������ꂽ�Ƃ��͑����Ă������Ă��悢
			}
			data.SetAngle(nowangle);
		}
		/*��]*/
		D3DXMatrixRotationY(&rotation, D3DXToRadian(data.GetAngle()));
	}
	TransDataPos(trans);

	//SOCK.SendPos(&posdata);																			//�ړ����̂݃T�[�o�[�ɍ��W�𑗐M

}

void Enemy::ChangeAnimation(int _animation, bool _roop, double _speed)
{
	if (m_anim.GetAnimeNo() == _animation) return;										//���݂̃A�j���[�V�����Ƃ������A�j���[�V�����������Ȃ�֐����甲����
	m_anim.ChangeAnimeSmooth(_animation, 0, 10, _roop);								//�A�j���[�V������_animation�ɕύX

}

bool Enemy::Damage()
{
	if (m_anim.GetAnimePos()==40 && data.GetAnimation() == DAMAGE) {
		data.SetAnimation(WAKEUP);
		standflg = true;
		if (CLIENT.DataEmpty() == false) {
			CLIENT.Lock();
			CLIENT.ClearData();										//�擾�f�[�^�S�폜
			CLIENT.Unlock();
		}
		return false;
	}

	D3DXMATRIX translation;												//�ړ��ʕ��̍��W������s��
	D3DXMatrixTranslation(&translation, 0, 0,acceleration.z);			//z�������̈ړ�������
	TransDataPos(translation);
	acceleration.z -= 0.003;											//�����x�����炷
	if (acceleration.z < 0)acceleration.z = 0.0f;

	m_mChara.CreateMove(data.GetPos());			//�v���C���[�̍��W�ݒ�
	m_mChara.RotateY_Local(data.GetAngle());							//angle����]
	m_anim.Animation(1);									// �A�j���[�V������i�s
	m_bcChara.CalcBoneMatrix();								// �S�{�[���ATransMat���g�p��LocalMat���X�V����
	return true;
}


void Enemy::TransDataPos(D3DXMATRIX _trans)
{
	D3DXVECTOR3 transpos(_trans._41, _trans._42, _trans._43);
	D3DXVECTOR3 pos(data.GetX(), data.GetY(), data.GetZ());
	pos += transpos;
	data.SetPos(pos);
}

D3DXMATRIX* Enemy::CreateMat()
{
	D3DXMATRIX returnmat;
	D3DXMatrixTranslation(&returnmat, data.GetX(), data.GetY(), data.GetZ());
	return &returnmat;
}
