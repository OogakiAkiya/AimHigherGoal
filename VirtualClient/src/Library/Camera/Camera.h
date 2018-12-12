#ifndef Camera_h
#define Camera_h

class Camera {
public:
	Camera(int _flg=ORIGINAL);
	~Camera();
	void ComplianceUpdate();					//�Ǐ]�J����
												
	//---------------------------------------------------------
	//���擾
	//---------------------------------------------------------
	D3DXVECTOR3* GetPos();
	D3DXVECTOR3* GetLook();
	D3DXVECTOR3* GetHead();
	D3DXMATRIX* GetView();
	D3DXMATRIX* GetProj();
	D3DXMATRIX GetRotMat();
	D3DXMATRIX* GetMat();
	void SetMat(D3DXMATRIX* _mat);

	//---------------------------------------------------------
	//�萔
	//---------------------------------------------------------
	static const int ORIGINAL = 1;
	static const int THREE_PERSON_PERSPECTiVE = 2;


private:
	//---------------------------------------------------------
	//�ϐ�
	//---------------------------------------------------------
	float angle;
	D3DXVECTOR3 pos;		//�J�����̍��W
	D3DXVECTOR3 look;		//�J�����̌���ʒu
	D3DXVECTOR3 head;		//�J�����̓��̌���
	D3DXMATRIX mat;			//�J�����s��
	D3DXMATRIX targetmat;	//�Ǐ]����Ώۂ̍s��
	D3DXMATRIX view;		//�r���[�s��
	D3DXMATRIX proj;		//�ˉe�s��

};

#endif