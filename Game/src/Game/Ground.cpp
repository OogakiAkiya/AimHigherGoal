#include"../Include.h"
#include"Ground.h"

Ground::Ground()
{
	//�����ݒ�
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z] = std::make_unique<Vertex>();
			surface[x][z]->LoadText("images/kabe.png", 256, 256, NULL);							//�n�ʂ̉摜�ǂݍ���
			surface[x][z]->InitVertex(SIZE, (float)X+x*SIZE,0.0f, (float)Z + z * SIZE);			//������
		}
	}

}

Ground::~Ground()
{
	//�������
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z] = nullptr;
		}
	}
}

void Ground::Draw()
{
	//�`��
	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			surface[x][z]->Draw();
		}
	}
}
