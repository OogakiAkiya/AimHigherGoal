#ifndef CharacterBase_h
#define CharacterBase_h

struct CharaStatus {
	int hp;							//�q�b�g�|�C���g
	int mp;							//�}�W�b�N�|�C���g
	char attack;					//�U����
	char defense;					//�h���
	char accurcy;					//������
	char movespeed;					//�ړ����x
};

class CharacterBase {
public:
	virtual ~CharacterBase() {};
	virtual void Update()=0;
	virtual void Render3D() = 0;
	virtual void Render2D() = 0;
	//---------------------------------------------------------
	//��񏊓�
	//---------------------------------------------------------
	D3DXMATRIX* GetMat();
	CharaStatus* Getstatus();
	std::shared_ptr<Camera> GetCamera();

	//---------------------------------------------------------
	//��񏑂�����
	//---------------------------------------------------------
	void SetMat(float _x, float _y, float _z);

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int WAIT = 0;											//�ҋ@
	static const int WALK = 1;											//����
	static const int RUN = 3;											//����
	static const int JUMPUP = 7;										//�W�����v�㏸
	static const int JUMPDOWN = 8;										//�W�����v����
	static const int LANDING = 9;										//���n
	static const int ATTACK = 11;										//�U��
	static const int DAMAGE = 13;										//�_���[�W���󂯂�
	static const int WAKEUP = 17;
protected:

	//---------------------------------------------------------
	//�ϐ�mat�̒l�����������邽�߂̊֐�
	//---------------------------------------------------------
	void Trans(float _x, float _y, float _z);
	void RotationLeft(float _rotation);
	void RotationRight(float _rotation);
	void Scale(float _x, float _y, float _z);

	//---------------------------------------------------------
	//����
	//---------------------------------------------------------
	float RayDecision();
	//---------------------------------------------------------
	//3D���f�����������߂̍s��
	//---------------------------------------------------------
	std::shared_ptr<Camera> camera;
	std::weak_ptr<XFILE> mesh;					//���b�V��
	D3DXMATRIX baseMat;					//�L�����N�^�[�̍s��
	CharaStatus status;				//�L�����N�^�[�X�e�[�^�X
	
};

#endif