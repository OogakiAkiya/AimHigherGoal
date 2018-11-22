#ifndef Vertex_h
#define Vertex_h

class Vertex
{
public:
	Vertex();
	~Vertex();
	void InitVertex(float _size,float _x, float _y, float _z);						//������(_size�͖ʂ̃T�C�Y_x_y_z�͖ʂ̕`�悷��ʒu)
	void LoadText(char _fname[], int _width, int _height, D3DCOLOR _color);			//�`�悷��摜���[�h(_fname�̓e�N�X�`���̃p�X,_width��,_hight����,_color���߂���FNULL�̎w��\)
	void Draw();																	//�`��

private:
	//---------------------------------------------------------
	// �\����
	//---------------------------------------------------------
	struct vertex{
		D3DXVECTOR3 Pos;						//���W
		D3DCOLOR Color;							//�F
		D3DXVECTOR2 Tex;						//�e�N�X�`���̈ʒu
	};

	//---------------------------------------------------------
	// �ϐ�
	//---------------------------------------------------------
	vertex v[4];								//���_
	D3DXMATRIX mat;								//���W�p�s��
	LPDIRECT3DTEXTURE9 texture;					//�e�N�X�`��

};
#endif