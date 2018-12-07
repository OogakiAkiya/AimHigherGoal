//#include "../../Include.h"
#include"../../StandardLibraryInclude.h"
#include"../../LibraryInclude.h"
#include"../../SimpleLib/SimpleLib.h"
using namespace SimpleLib;

#include "ToonShader.h"



void ToonShader::Init()
{
	Release();

	//===============================================
	// エフェクト(シェーダ)ファイルのロード
	//  今回はコンパイル済みシェーダファイル(.cso)を読み込んでいますが、
	//  ソースファイルである.fxをそのまま読み込むこともできます(この場合は読み込み&コンパイルになるので、ちょっと時間がかかる)。
	//  もちろん.csoファイルを読み込むほうを推奨します。
	//===============================================
	LPD3DXBUFFER pErr = nullptr;
	HRESULT hr;
	hr = D3DXCreateEffectFromFile(
		DEV, "Shader/ToonShader.cso",
		nullptr, nullptr, 0, nullptr, &m_Effect, &pErr);

	if(hr != D3D_OK){
		MessageBox(nullptr,(LPCSTR)pErr->GetBufferPointer(),"シェーダコンパイルエラー",MB_OK);
		return;
	}

	// 1x1の白テクスチャを作成
	m_texDot.CreateTexture(DEV, 1,1,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED);
	m_texDot.Fill<UINT>(0xFFFFFFFF);	// 白で塗りつぶす

	m_texDotNormal.CreateTexture(DEV, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
	m_texDotNormal.Fill<UINT>(0xFF7F7FFF);	// 白で塗りつぶす

	//テクスチャ作成
	m_texToonDefault.LoadTexture(DEV, "images/Toon.png");
	m_Effect->SetTexture("g_ToonTex", m_texToonDefault.GetTex());

}

void ToonShader::DrawMesh(CMesh *pMesh)
{
	m_Effect->SetInt("g_CurNumBones", 0);

	// テクニック選択
	m_Effect->SetTechnique("Tech");
	// テクニック開始
	m_Effect->Begin(0, 0);
	// パス開始
	m_Effect->BeginPass(0);


	// マテリアルの数ぶん描画
	for(int i=0; i < pMesh->GetMaterialCnt(); i++ )
	{
		//--------------------------
		// マテリアルのデータをシェーダへセット
		//--------------------------
		D3DMATERIAL9&		mate = pMesh->GetMaterials()[i];			// マテリアルを取得
		SetMaterial(mate, *pMesh->GetTextures()[i], *pMesh->GetNormalTextures()[i]);

		//--------------------------
		// シェーダへデータのセットを確定させる
		//--------------------------
		// ※これを呼ばないと、シェーダ側にうまくパラメータがセットされないよ！
		m_Effect->CommitChanges();

		//--------------------------
		// メッシュを描画
		//--------------------------
		pMesh->GetMesh()->DrawSubset( i );
	}

	// パス終了
	m_Effect->EndPass();
	// テクニック終了
	m_Effect->End();
}

void ToonShader::DrawMeshContainer(CBoneController* pBc, D3DXMESHCONTAINER_EX* pMc)
{
	// スキンメッシュ
	if (pMc->pSkinInfo)
	{
		// テクニック選択
		m_Effect->SetTechnique("Tech");
		UINT numPasses;
		// テクニック開始
		m_Effect->Begin(&numPasses, 0);
		// パス開始
		m_Effect->BeginPass(0);

		// ボーン行列作業用配列
		static std::vector<D3DXMATRIX> workBoneMatArray;
		if (workBoneMatArray.size() < pMc->m_NumPaletteEntries) {
			workBoneMatArray.resize(pMc->m_NumPaletteEntries);
		}

		UINT offID;
		UINT iMatrixIndex;
		LPD3DXBONECOMBINATION pBoneCombination = reinterpret_cast<LPD3DXBONECOMBINATION>(pMc->m_pBoneCombinationBuf->GetBufferPointer());
		for (UINT iAttrib = 0; iAttrib < pMc->m_dwBoneCombiCnt; iAttrib++)
		{
			// 影響する行列配列を用意する
			for (UINT iPaletteEntry = 0; iPaletteEntry < pMc->m_NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneCombination[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					offID = pMc->m_BoneFrameArray[iMatrixIndex]->m_OffsetID;
					workBoneMatArray[iPaletteEntry] = pBc->GetBoneTree()[offID]->OffsetLocalMat;
				}
			}

			// ボーン行列配列をセット
			m_Effect->SetMatrixArray("g_mWArray", &workBoneMatArray[0], pMc->m_NumPaletteEntries);

			// マテリアル
			SetMaterial(pMc->pMaterials[ pBoneCombination[iAttrib].AttribId ].MatD3D,
						*pMc->m_Textures[pBoneCombination[iAttrib].AttribId],
						*pMc->m_NormalTextures[pBoneCombination[iAttrib].AttribId]
						);

			// ボーン数によって頂点シェーダの処理を選ぶためのフラグをセット
			m_Effect->SetInt("g_CurNumBones", pMc->m_dwWeight);

			// ※これを呼ばないと、シェーダ側にうまくパラメータがセットされないよ！
			m_Effect->CommitChanges();

			// サブセットを描画
			pMc->MeshData.pMesh->DrawSubset(iAttrib);
		}
		// テクニック終了
		m_Effect->EndPass();
		// パス終了
		m_Effect->End();
	}
	// スタティックメッシュ
	else {
		// スタティック描画
		m_Effect->SetTechnique("Tech");

		UINT passNum;
		m_Effect->Begin(&passNum, 0);
		m_Effect->BeginPass(0);

		// マテリアルの数ぶんループ
		for (UINT i = 0; i < pMc->NumMaterials; i++)
		{
			// マテリアルセット
			SetMaterial( pMc->pMaterials[i].MatD3D, *pMc->m_Textures[i], *pMc->m_NormalTextures[i] );

			// ※これを呼ばないと、シェーダ側にうまくパラメータがセットされないよ！
			m_Effect->CommitChanges();

			// メッシュを描画
			pMc->MeshData.pMesh->DrawSubset(i);
		}

		m_Effect->EndPass();
		m_Effect->End();
	}
}

void ToonShader::SetMaterial(const D3DMATERIAL9& mate, const CTexture& texDiffuse, const CTexture& texNormal)
{
	// ディフューズ
	m_Effect->SetVector("g_MateDiffuse", (D3DXVECTOR4*)&mate.Diffuse);

	// スペキュラ
	m_Effect->SetVector("g_MateSpecular", (D3DXVECTOR4*)&mate.Specular);
		
	// スペキュラPower
	m_Effect->SetFloat("g_MateSpePower", mate.Power);

	// エミッシブ
	m_Effect->SetVector("g_MateEmissive", (D3DXVECTOR4*)&mate.Emissive);

	// ※マテリアルのAmbientは使わない


	//--------------------------
	// テクスチャをシェーダへセット
	//--------------------------
	LPDIRECT3DTEXTURE9	pTex;

		
	// テクスチャ
	pTex = texDiffuse.GetTex();
	if (pTex == nullptr)pTex = m_texDot.GetTex();	// テクスチャが無いときは、1x1白テクスチャを使用

	m_Effect->SetTexture("g_MateDiffuseTex", pTex);

	// 法線マップ
	if (m_ForceSetNormalTex) {
		m_Effect->SetTexture("g_MateNormalTex", m_ForceSetNormalTex->GetTex());
	}
	else {
		pTex = texNormal.GetTex();
		if (pTex == nullptr)pTex = m_texDotNormal.GetTex();
		m_Effect->SetTexture("g_MateNormalTex", pTex);
	}
}
