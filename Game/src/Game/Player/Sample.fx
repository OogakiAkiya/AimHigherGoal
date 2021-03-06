//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//
// グローバル定数
//　※VC側から、好きな値をセットできる唯一の場所
//
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

//-----------------------------------
// 変換行列
//-----------------------------------
float4x4 g_mW;		// ワールド行列　つまりキャラの行列
float4x4 g_mV;		// ビュー行列
float4x4 g_mP;		// 射影行列

//-----------------------------------
// マテリアル関係
//-----------------------------------
float4	g_MateDiffuse = float4(1,1,1,1);	// マテリアルの拡散(Diffuse)色
float4	g_MateSpecular = float4(1,1,1,1);	// マテリアルの反射(Specular)色 ＝ 光が金属のように反射するときの色
float	g_MateSpePower = 50;				// マテリアルの反射(Specular)の力
float4  g_MateEmissive = float4(0,0,0,0);   // マテリアルの自己発光色　このサンプルでは未使用です
// ※マテリアルのAmbientは使わない

//-----------------------------------
// テクスチャ
//-----------------------------------
texture g_MateDiffuseTex;         // マテリアルのテクスチャ
// MateDiffuseTex用のサンプラ
// ※実際にテクスチャから色を取得するには、こいつを使う
sampler g_MateDiffuseSmp = sampler_state
{
	Texture = <g_MateDiffuseTex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

texture g_MateNormalTex; // マテリアルのテクスチャ
// MateDiffuseTex用のサンプラ
// ※実際にテクスチャから色を取得するには、こいつを使う
sampler g_MateNormalSmp = sampler_state
{
	Texture = <g_MateNormalTex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//-----------------------------------
// ライト関係
//-----------------------------------
// 平行光源
int    g_LightEnable = 1;
float3 g_LightDir = float3(0 ,-1, 0);	    // 平行光の方向
float4 g_LightColor = float4(1, 1, 1, 1); // 平行光のDiffuse色

// 環境光
float4 g_LightAmbient;                    // 環境光の色

//-----------------------------------
// カメラ用
//-----------------------------------
float3 g_CamPos; // カメラ座標





//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//
// ここから頂点シェーダ
//
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

//==============================================================
// 頂点シェーダからピクセルシェーダに渡すデータ用構造体
//==============================================================
struct VS_OUT
{
	float4 Pos		: POSITION;		// 2D座標
	float2 UV	    : TEXCOORD0;	// テクスチャUV
	
	float3 wPos		: TEXCOORD1;	// 3D座標
	float3 wN		: TEXCOORD2;	// 法線

	float3 wT       : TEXCOORD3;
	float3 wB       : TEXCOORD4;
	float3 wvPos    : TEXCOORD5;
};

//==============================================================
// 頂点シェーダ
//	・頂点が１つ送られてくるので、それを処理しピクセルシェーダに渡す。
//==============================================================
VS_OUT VS (
	float4 pos : POSITION,				// 座標(モデルの頂点座標がそのまま来る　つまり全くキャラの行列とか考慮されてません＝「ローカル座標」といいます)
	float3 normal : NORMAL,             // 法線(モデルの法線がそのまま来る　つまり全くキャラの行列とか考慮されてません)
	float2 uv : TEXCOORD0			// テクスチャUV
) {
	VS_OUT Out = (VS_OUT)0;		// ピクセルデータに渡すデータ用構造体

	//-------------------------------------
	// 頂点座標を2D(射影座標系)へ変換
	//-------------------------------------

	//pos.xyz += normal*0.2;			//砲台の大きさが変わる(頂点シェーダーを使うと頂点の大きさが変わる)

	Out.Pos = mul(pos, g_mW);       // ワールド変換　キャラの行列で変換する＝「ワールド座標」といいます
		Out.wPos = Out.Pos.xyz;	    // ワールド座標をピクセルシェーダへ渡す
	Out.Pos = mul(Out.Pos, g_mV);	// ビュー変換　カメラを考慮した3D座標になる＝「ビュー座標」といいます
		Out.wvPos = Out.Pos.xyz; 
	Out.Pos = mul(Out.Pos, g_mP);	// -1〜1の2D空間での座標になる＝「射影座標」といいます

	// テクスチャUV座標をそのままピクセルシェーダへ(ここを変数すれば、UVスクロールとかもできる)
	Out.UV = uv;

	//--------------------------------------
	// 簡易版 法線マップ用ベクトル生成
	//--------------------------------------
	Out.wN = normalize(mul(normal, (float3x3) g_mW)); // ローカル法線をワールド法線に変換
	// X軸を求めるため、[0,1,0]とZ軸(wN)の外積を計算するが、wNが真上(0, 1, 0)の場合は結果が[0,0,0]になってしまう。
	// なので、wNが[0,1,0]の近い場合は、別の方向と外積をすることで対処。
	// ※ifより、以下のような条件文のほうがパフォーマンスが良い
	float3 vUp = abs(dot(Out.wN, float3(0, 1, 0))) <= 0.999999 ? float3(0, 1, 0) : float3(0, 0, -1);    // wNが[0,1,0]に限りなく近い場合は、(0,0,-1)で外積する。
	Out.wT = normalize(cross(vUp, Out.wN));
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//
// ここからピクセルシェーダ
//
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

//==================================================================
// ピクセルシェーダから出力するデータ用構造体
//==================================================================
struct PS_OUT
{
	float4 Color : COLOR0;  // 出力色(RenderTargetの0番目に出力する)
};

// ピクセルシェーダ
//	・ピクセルの色を決定し、その色を返す(R,G,B,Aのfloat4型)
PS_OUT PS(VS_OUT In)
{
	// 目(カメラ)への方向
	float3 vCam = normalize(g_CamPos - In.wPos);

	//=========================================================================
	// 法線マッピング
	//=========================================================================

	// 頂点シェーダから出力してきた３つの法線を行列化
	float3x3 mTBN = { 
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};

	// 法線マップから法線データを取得
	float3 normal = tex2D(g_MateNormalSmp, In.UV).xyz;
	normal = normal * 2.0 - 1.0;            // 0〜1 のデータを -1〜1 へ変換

	// 法線をピクセル方向に向ける
	float3 w_normal = normalize( mul(normal, mTBN) );

	// いつもの方法(頂点の法線をそのまま使う方法)
//	float3 w_normal = normalize(In.wN);

	//=========================================================================
	//
	// モデル色 … このピクセルでのモデルの材質の色を求める。通常はマテリアル色とテクスチャ色を合成したものでいい。
	//
	//=========================================================================
	float4 texCol = tex2D(g_MateDiffuseSmp, In.UV); // テクスチャ色取得
	float4 modelCol = g_MateDiffuse * texCol; // マテリアルの色 * テクスチャの色

	// アルファテスト(テクスチャの透明度が一定以下だと、このピクセル色は出力しない)
	if (texCol.a < 0.1)discard;

	//=========================================================================
	//
	// ライト色 … このピクセルが受けるライトの色を求める。
	//
	//=========================================================================
	float3 lightCol = 0;        // すべてのライトの合計拡散色
	float3 lightSpe = 0;        // すべてのライトの合計反射色

	if (g_LightEnable)
	{
		//================================================
		//
		// 平行光でのライティング
		//
		//================================================
		{
			//-----------------------------------
			// 拡散光(Diffuse) … 物質に吸収され、再び飛び出てきた光。光の色は物質の色に染まる。
			// ランバート照明法
			//-----------------------------------
			float LPow = dot(-g_LightDir, w_normal); // 光の強さ -1〜1
			LPow = saturate(LPow); // 0〜1内に収める このLPowが、このピクセルに当たる光の強さになる　0だと0%  1だと100%


			//-----------------------------------
			// シャドウマップ判定
			//-----------------------------------





			//-----------------------------------
			// 光の色 * 光の強さ
			//-----------------------------------
			lightCol += g_LightColor.rgb * LPow;

			//-----------------------------------
			// 反射光(Specular) … 金属のように反射する光。光の色そのまま反射される(例外あり)。
			// Blinn Phong
			//-----------------------------------
			float3 vH = normalize(-g_LightDir + vCam); // ハーフベクトル
			float spePow = saturate(dot(vH, w_normal)); // 
			spePow = pow(spePow, g_MateSpePower + 1);


			// ライト色 * 反射の強さ
			lightSpe += g_LightColor.rgb * spePow;
		}

		//------------------------------------------------
		// ポイントライト
		//------------------------------------------------







		//------------------------------------------------
		// 環境色 … どんな状態でも絶対に当たる光のこと。これが無いと暗すぎたりする。
		//------------------------------------------------
		// 環境光ぶんライト色を加算する
		lightCol += g_LightAmbient.rgb;
	}
	else
	{
		lightCol = 1;
	}

	// 拡散色
	float4 diffuseCol = modelCol; // 拡散光はモデルの拡散色に影響する。最終的な拡散(Diffuse)色を求める。
	diffuseCol.rgb *= lightCol;

	// 反射色
	float4 speCol;
	speCol.rgb = lightSpe * g_MateSpecular.rgb; // 反射光はモデルの反射色に影響する。最終的な反射(Specular)色を求める。
	speCol.a = 0;
//	speCol.a = (speCol.r + speCol.g + speCol.b) / 3.0; // スペキュラが強いほど不透明にする例(ガラス玉のような表現ができる)
	

	//================================================
	// 最終的な色を算出
	//================================================

	PS_OUT Out;
	Out.Color = diffuseCol + speCol * 3; // 拡散色 + 反射色

	// 自己発光(エミッシブ)も足す
	Out.Color.rgb += g_MateEmissive.rgb * 4;    // エミッシブはメタセコイヤで0〜1の範囲で設定できるが、もっと光らせたい時があるので4倍くらいしておく

	// α値は１を超えると、後々まずいことがあるので0〜1の範囲に収める
	Out.Color.a = saturate(Out.Color.a);

	//================================================
	// 距離フォグ
	//================================================





	//================================================
	// 色を出力
	//================================================

	//----------------------------------------------------------------------------------------------------------
	//Out.Color = float4(0.8, 0.8, 0.8,0.2);		//ピクセルシェーダーをいじるとピクセルの色を弄ることが出来る
	//Out.Color.xyz = w_normal;
	//----------------------------------------------------------------------------------------------------------
	return Out;		//Outで色を出力
}



//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// テクニック
//  描画の詳細をまとめたもの。
//  つまり、使用したい頂点シェーダとピクセルシェーダを指定したり、
//  RenderStateを指定したりできる。
//
//  ※VC側からは、このテクニックを使って描画を行う感じになる
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
technique Tech
{
	pass P0 {
		VertexShader = compile vs_3_0 VS();   // 頂点シェーダはVS()を使う
		PixelShader = compile ps_3_0 PS();    // ピクセルシェーダはPS()を使う
	}
}
