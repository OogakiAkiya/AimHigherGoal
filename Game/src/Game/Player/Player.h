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
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void State();														//�L�����̗����Ă���t���O���ƂɈړ���W�����v�̏������Ăяo��
	void Attack();														//�U������
	void Jump();														//�W�����v����
	void Wait();														//���͂��Ȃ��Ƃ�
	void Move();														//�ړ�
	void Damage();														//�_���[�W	
	void WakeUp();														//�N���オ��
	void Rotation(D3DXVECTOR3 _vec);									//��]
	void ChangeAnimation(int _animation, bool _roop,float _speed=1.0f);	//�A�j���[�V�����̕ύX
	void TransDataPos(D3DXMATRIX _trans);								//���W�Z�b�g
	void TransDataPos(D3DXVECTOR3* _vec);								//���W�Z�b�g
	D3DXMATRIX* CreateMat();											//���W�o��
	D3DXVECTOR3* GetVector(float _x, float _y, float _z);				//�v���C���[�̌����Ă������������������Ɉړ������Ƃ��̃x�N�g�������߂�

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	//�����I�ɂȂ��Ȃ�
	static const int JUMPRESET = 0;
	static const int JUMPUPER = 1;
	static const int JUMPDOWNER = 2;
	static const int JUMPSTANDING = 3;
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	//�X�L�����b�V��
	CBoneController	skinBone;
	CMatrix			skinChara;
	CAnimator		skinAnimation;

	//�V�F�[�_
	ToonShader toonShader;

	Data* data;
	char jumpFlg = 0;
	bool attackFlg = false;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//�����x
};

static const float MOVESPEEDLIMIT = 0.08;
#endif