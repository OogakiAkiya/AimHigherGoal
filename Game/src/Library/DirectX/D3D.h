#ifndef D3D_h
#define D3D_h

class D3D
{
public:
	int Init(HWND _hwnd, bool _full = false);
	void SceneBegin();
	void SceneEnd();

	void SpriteBegin();
	void SpriteEnd();

	void Release();


	LPDIRECT3DDEVICE9 GetDevice();
	LPD3DXSPRITE GetSprite();
	D3DLIGHT9 GetLight();
	LPD3DXFONT GetFont();
	// �V���O���g��
	static D3D & GetInstance();
	static void DeleteInstance();



	//�}�N����static const�ŋL�q
	static const int SCRW = 1280;	// �E�B���h�E���iWidth
	static const int SCRH = 720;		// �E�B���h�E�����iHeight
	static const int	FVF_VERTEX = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);

private:
	D3D();
	~D3D();
	LPDIRECT3D9				lpD3D;		// Direct3D�C���^�[�t�F�C�X
	LPDIRECT3DDEVICE9		lpD3DDevice;	// Direct3DDevice�C���^�[�t�F�C�X
	D3DPRESENT_PARAMETERS d3dpp;
	D3DLIGHT9 light;
	LPD3DXSPRITE lpSprite;	// �X�v���C�g
	LPD3DXFONT lpFont;		// �t�H���g
	static D3D* s_Instance;

	D3D & operator=(D3D &);
	D3D(D3D&);

};
#define DX D3D::GetInstance()
#define DEV DX.GetDevice()
#define DXSP DX.GetSprite()

#endif