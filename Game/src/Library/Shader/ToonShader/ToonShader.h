#ifndef ToonShader_h
#define ToonShader_h

//======================================================
// サンプルシェーダ
//	Sample.fxを動作させるためのクラス
//======================================================
class ToonShader{
public:
	LPD3DXEFFECT	m_Effect;		// エフェクト

	CTexture		m_texDot;		// 1x1 白のテクスチャ用(テクスチャが無いマテリアルの代用品)
	CTexture		m_texDotNormal;	// 1x1 Z方向を向きの法線マップ

	CTexture*		m_ForceSetNormalTex = nullptr;	// ここにテクスチャを入れると、強制的にこれを使用する(マテリアルの法線マップは使用されなくなる)

	// 
	ToonShader(){
		m_Effect = nullptr;
	}
	~ToonShader(){
		Release();
	}

	//======================================================
	// エフェクト読み込み＆解放など
	//======================================================
	// 初期設定
	void Init();

	// 解放
	void Release()
	{
		SAFE_RELEASE(m_Effect);
		m_texDot.Release();
	}

	//======================================================
	// パラメータ設定系
	//======================================================
	// シェーダに行列を設定
	// ワールド
	void SetTransformWorld(const D3DXMATRIX* mat){
		m_Effect->SetMatrix("g_mW", mat);
	}
	// ビュー
	void SetTransformView(const D3DXMATRIX* mat){
		m_Effect->SetMatrix("g_mV", mat);
	}
	// 射影
	void SetTransformProj(const D3DXMATRIX* mat){
		m_Effect->SetMatrix("g_mP", mat);
	}

	// ライト情報セット
	void SetDirectionalLight(D3DLIGHT9* light) {
		// 平行光の方向
		m_Effect->SetValue("g_LightDir", &light->Direction, sizeof(D3DXVECTOR3));
		// 拡散色(ディフューズ)
		m_Effect->SetVector("g_LightColor", (D3DXVECTOR4*)&light->Diffuse);
		// 環境色(アンビエント)
		m_Effect->SetVector("g_LightAmbient", (D3DXVECTOR4*)&light->Ambient);
		// 反射色(スペキュラ)
		m_Effect->SetVector("g_LightSpecular", (D3DXVECTOR4*)&light->Specular);
	}

	// ライトOn/Off
	void SetLightEnable(int enable) {
		// 有効
		m_Effect->SetInt("g_LightEnable", enable);
	}

	// カメラ位置設定
	void SetCamPos(const D3DXVECTOR3* lpvPos){
		m_Effect->SetValue("g_CamPos", lpvPos,sizeof(D3DXVECTOR3));
	}

	//======================================================
	// 描画係
	//======================================================
	// メッシュ描画
	void DrawMesh(CMesh *pMesh);

	void DrawMesh(CBoneController *pBc) {
		for (UINT i = 0; i<pBc->GetSkinMesh()->GetMeshContainerArray().size(); i++)
		{
			DrawMeshContainer(pBc, pBc->GetSkinMesh()->GetMeshContainerArray()[i]);
		}
	}

private:
	CTexture m_texToonDefault;				//外部テクスチャ用のファイルを読み込む

	// スキンメッシュ描画用
	void DrawMeshContainer(CBoneController* pBc, D3DXMESHCONTAINER_EX* pMc);

	void SetMaterial(const D3DMATERIAL9& mate, const CTexture& texDiffuse, const CTexture& texNormal);

};

#endif
