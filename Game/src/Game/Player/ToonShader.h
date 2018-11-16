#ifndef ToonShader_h
#define ToonShader_h

//======================================================
// �T���v���V�F�[�_
//	Sample.fx�𓮍삳���邽�߂̃N���X
//======================================================
class ToonShader{
public:
	LPD3DXEFFECT	m_Effect;		// �G�t�F�N�g

	CTexture		m_texDot;		// 1x1 ���̃e�N�X�`���p(�e�N�X�`���������}�e���A���̑�p�i)
	CTexture		m_texDotNormal;	// 1x1 Z�����������̖@���}�b�v

	CTexture*		m_ForceSetNormalTex = nullptr;	// �����Ƀe�N�X�`��������ƁA�����I�ɂ�����g�p����(�}�e���A���̖@���}�b�v�͎g�p����Ȃ��Ȃ�)

	// 
	ToonShader(){
		m_Effect = nullptr;
	}
	~ToonShader(){
		Release();
	}

	//======================================================
	// �G�t�F�N�g�ǂݍ��݁�����Ȃ�
	//======================================================
	// �����ݒ�
	void Init();

	// ���
	void Release()
	{
		SAFE_RELEASE(m_Effect);
		m_texDot.Release();
	}

	//======================================================
	// �p�����[�^�ݒ�n
	//======================================================
	// �V�F�[�_�ɍs���ݒ�
	// ���[���h
	void SetTransformWorld(const D3DXMATRIX* mat){
		m_Effect->SetMatrix("g_mW", mat);
	}
	// �r���[
	void SetTransformView(const D3DXMATRIX* mat){
		m_Effect->SetMatrix("g_mV", mat);
	}
	// �ˉe
	void SetTransformProj(const D3DXMATRIX* mat){
		m_Effect->SetMatrix("g_mP", mat);
	}

	// ���C�g���Z�b�g
	void SetDirectionalLight(D3DLIGHT9* light) {
		// ���s���̕���
		m_Effect->SetValue("g_LightDir", &light->Direction, sizeof(D3DXVECTOR3));
		// �g�U�F(�f�B�t���[�Y)
		m_Effect->SetVector("g_LightColor", (D3DXVECTOR4*)&light->Diffuse);
		// ���F(�A���r�G���g)
		m_Effect->SetVector("g_LightAmbient", (D3DXVECTOR4*)&light->Ambient);
		// ���ːF(�X�y�L����)
		m_Effect->SetVector("g_LightSpecular", (D3DXVECTOR4*)&light->Specular);
	}

	// ���C�gOn/Off
	void SetLightEnable(int enable) {
		// �L��
		m_Effect->SetInt("g_LightEnable", enable);
	}

	// �J�����ʒu�ݒ�
	void SetCamPos(const D3DXVECTOR3* lpvPos){
		m_Effect->SetValue("g_CamPos", lpvPos,sizeof(D3DXVECTOR3));
	}

	//======================================================
	// �`��W
	//======================================================
	// ���b�V���`��
	void DrawMesh(CMesh *pMesh);

	void DrawMesh(CBoneController *pBc) {
		for (UINT i = 0; i<pBc->GetSkinMesh()->GetMeshContainerArray().size(); i++)
		{
			DrawMeshContainer(pBc, pBc->GetSkinMesh()->GetMeshContainerArray()[i]);
		}
	}

private:
	CTexture m_texToonDefault;				//�O���e�N�X�`���p�̃t�@�C����ǂݍ���

	// �X�L�����b�V���`��p
	void DrawMeshContainer(CBoneController* pBc, D3DXMESHCONTAINER_EX* pMc);

	void SetMaterial(const D3DMATERIAL9& mate, const CTexture& texDiffuse, const CTexture& texNormal);

};

#endif
