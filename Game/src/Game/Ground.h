#ifndef Ground_h
#define Ground_h
class Ground 
{
public:
	Ground();
	~Ground();
	void Draw();				//�`��
private:
	//�萔
	static const int SIZE = 10;
	static const int X =- 50;
	static const int Z =- 50;

	Vertex* surface[10][10];	//�n�ʗp�̖ʗp�z��
};
#endif