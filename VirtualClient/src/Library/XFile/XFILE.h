#ifndef XFILE_h		//ModelShader_h�͊e�w�b�_�[���Ƃɕς��Ȃ��ƃG���[���o��
#define XFILE_h


class XFILE {
public:
	XFILE();
	~XFILE();
	
	void Load(LPDIRECT3DDEVICE9 _lpD3DDevice,std::string _pass);
	void Draw(LPDIRECT3DDEVICE9 _lpD3DDevice,D3DXMATRIX *_World);
	void Release();
private:
	//---------------------------------------------------------
	// �ϐ�
	//---------------------------------------------------------
	LPD3DXMESH lpMesh = NULL;			//���b�V���̌`��
	DWORD numMaterial = 0;				//�}�e���A���̐�
	D3DMATERIAL9* mat = nullptr;			//�}�e���A���̓��e
	LPDIRECT3DTEXTURE9* texture = nullptr;		//�e�N�X�`��

};
#endif