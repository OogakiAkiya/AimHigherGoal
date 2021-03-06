#include"../Include.h"
#include"Ground.h"

Ground::Ground()
{
	//初期設定
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z] = std::make_unique<Vertex>();
			surface[x][z]->LoadText("images/kabe.png", 256, 256, NULL);							//地面の画像読み込み
			surface[x][z]->InitVertex(SIZE, (float)X+x*SIZE,0.0f, (float)Z + z * SIZE);			//初期化
		}
	}

}

Ground::~Ground()
{
	//解放処理
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z] = nullptr;
		}
	}
}

void Ground::Draw()
{
	//描画
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z]->Draw();
		}
	}
}
