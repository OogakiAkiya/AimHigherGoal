#ifndef Ground_h
#define Ground_h
class Ground 
{
public:
	Ground();
	~Ground();
	void Draw();				//描画
private:
	//定数
	static const int SIZE = 10;
	static const int X =- 50;
	static const int Z =- 50;

	Vertex* surface[10][10];	//地面用の面用配列
};
#endif