#ifndef Player_h
#define Player_h

class Player :public CharacterBase
{
public:
	Player();
	~Player() override;
	virtual void Update() override;
	virtual void Render2D() override;
	virtual void Render3D() override;

private:	
	void State();														//�L�����̗����Ă���t���O���ƂɈړ���W�����v�̏������Ăяo��
	void Attack();														//�U������
	void Jump();														//�W�����v����
	void Wait();														//���͂��Ȃ��Ƃ�
	void Move();														//�ړ�
	void Damage();
	void WakeUp();
	void Rotation(D3DXVECTOR3 _vec);									//��]
	void ChangeAnimation(int _animation, bool _roop,float _speed=1.0);
	void TransDataPos(D3DXMATRIX _trans);
	void TransDataPos(D3DXVECTOR3* _vec);

	D3DXMATRIX* CreateMat();
	D3DXVECTOR3* GetVector(float _x, float _y, float _z);				//�v���C���[�̌����Ă������������������Ɉړ������Ƃ��̃x�N�g�������߂�

	//�X�L�����b�V��
	CBoneController	m_bcChara;
	CMatrix			m_mChara;
	CAnimator		m_anim;

	//�V�F�[�_
	ToonShader m_toonshader;
	Data data;
	char jumpflg = 0;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//�����x
};

static const float MOVESPEEDLIMIT = 0.08;
#endif