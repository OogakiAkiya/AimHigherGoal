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
	Camera* GetCamera();
private:	
	void State();														//�L�����̗����Ă���t���O���ƂɈړ���W�����v�̏������Ăяo��
	void Attack();														//�U������
	void Jump();														//�W�����v����
	void Wait();														//���͂��Ȃ��Ƃ�
	void Move();														//�ړ�
	void Rotation(D3DXVECTOR3 _vec);									//��]
	void ChangeAnimation(int _animation, bool _roop,double _speed=1.0);
	bool Damage();
	void TransDataPos(D3DXMATRIX _trans);
	D3DXMATRIX* CreateMat();


	//�X�L�����b�V��
	CBoneController	m_bcChara;
	CMatrix			m_mChara;
	CAnimator		m_anim;

	//�V�F�[�_
	ToonShader m_toonshader;

	//Camera* camera;
	int enemyid;
	Data data;
	char jumpflg = 0;
	D3DXMATRIX initmat;
	D3DXVECTOR3 acceleration = { 0.001f,0.0f,0.0f };					//�����x
	bool standflg = false;												//�_���[�W���󂯂Ă��痧�܂�

};

//static const float MOVESPEEDLIMIT = 0.08;

#endif