#ifndef Mouse_h
#define Mouse_h

class Mouse 
{
public:
	void ThirdPersonPointUpdata();					//�O�l�̎��_�̎��Ɏg�p����Ƃ悢Update(�}�E�X�J�[�\���͒����ɌŒ�)
	void FreeUpdate();								//�}�E�X�J�[�\������ʓ������R�Ɉړ��ł���

	//���擾
	POINT GetMousePt();								//MousePt�擾
	float GetMoveX();								//x�����̈ړ��ʂ��擾
	float GetMoveY();								//y�����̈ړ��ʂ��擾

	// �V���O���g��
	static Mouse & GetInstance();					//�C���X�^���X�̎擾
	static void DeleteInstance();					//�C���X�^���X�̍폜

private:
	Mouse();
	~Mouse();

	//�萔�錾
	static const int SENSITIVITY=2;	

	static Mouse* s_Instance;						//�C���X�^���X
	float movey=0, movex=0;								//�}�E�X��x�Ey�̈ړ���
	POINT mouseBasePt;								//�}�E�X�̍��W�擾

};
#define MOUSE Mouse::GetInstance()

#endif