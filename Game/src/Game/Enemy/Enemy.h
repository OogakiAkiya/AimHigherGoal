#ifndef Enemy_h
#define Enemy_h

class Enemy :public CharacterBase
{
public:
	Enemy(int _id);
	~Enemy();
	void Update();
	void Render2D();
	void Render3D();

	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	Camera* GetCamera();

private:
	//---------------------------------------------------------
	//���[�J���֐�
	//---------------------------------------------------------
	void State();														//�L�����̗����Ă���t���O���ƂɈړ���W�����v�̏������Ăяo��
	void Attack();														//�U������
	void Jump();														//�W�����v����
	void Wait();														//���͂��Ȃ��Ƃ�
	void Move();														//�ړ�
	void Rotation(D3DXVECTOR3 _vec);									//��]
	void ChangeAnimation(int _animation, bool _roop,double _speed=1.0);	//�A�j���[�V�����̕ύX
	bool Damage();														//�U�����󂯂�
	void TransDataPos(D3DXMATRIX _trans);								//D3DXMATRIX�œn�����l��ϊ������W�ɃZ�b�g
	D3DXMATRIX* CreateMat();											//���W��D3DXMATRIX�œn��


	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	//�X�L�����b�V��
	CBoneController	skinBoon;
	CMatrix			skinChara;
	CAnimator		skinAnimation;

	//�V�F�[�_
	ToonShader toonShader;

	//Camera* camera;
	int enemyId;
	Data data;
	char jumpFlg = 0;
	D3DXMATRIX initMat;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//�����x
	bool standFlg = false;												//�_���[�W���󂯂Ă��痧�܂�

};

//static const float MOVESPEEDLIMIT = 0.08;

#endif