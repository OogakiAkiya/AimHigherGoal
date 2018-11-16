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
	LPD3DXMESH lpMesh = NULL;			//���b�V���̌`��
	DWORD NumMaterial = 0;			//�}�e���A���̐�
	D3DMATERIAL9 *Mat = NULL;			//�}�e���A���̓��e
	LPDIRECT3DTEXTURE9 *Tex = NULL;	//�e�N�X�`���̓��e

};
#endif