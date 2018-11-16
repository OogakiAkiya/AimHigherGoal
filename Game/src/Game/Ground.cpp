#include"../Include.h"
#include"Ground.h"

Ground::Ground()
{
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z] = new Vertex();
			surface[x][z]->LoadText("images/kabe.png", 256, 256, NULL);							//地面の画像読み込み
			surface[x][z]->InitVertex(SIZE, (float)X+x*SIZE,0.0f, (float)Z + z * SIZE);			//初期化
		}
	}

}

Ground::~Ground()
{
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			delete surface[x][z];							//デリート
		}
	}
}

void Ground::Draw()
{
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z]->Draw();							//描画
		}
	}
}
