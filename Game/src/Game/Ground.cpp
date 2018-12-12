#include"../Include.h"
#include"Ground.h"

Ground::Ground()
{
	//‰Šúİ’è
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z] = std::make_unique<Vertex>();
			surface[x][z]->LoadText("images/kabe.png", 256, 256, NULL);							//’n–Ê‚Ì‰æ‘œ“Ç‚İ‚İ
			surface[x][z]->InitVertex(SIZE, (float)X+x*SIZE,0.0f, (float)Z + z * SIZE);			//‰Šú‰»
		}
	}

}

Ground::~Ground()
{
	//‰ğ•úˆ—
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z] = nullptr;
		}
	}
}

void Ground::Draw()
{
	//•`‰æ
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z]->Draw();
		}
	}
}
