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

// スキンメッシュ用
static const int MAX_MATRICES = 26; // 最大ボーン数
float4x3 g_mWArray[MAX_MATRICES]; // ボーン行列配列
int g_CurNumBones = 2; // 影響ボーン数

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


//テクスチャ
//トゥーン
texture g_ToonTex;						//陰影用のテクスチャ(ModelShader.hに追加するまでテクスチャがセットされていない)
sampler g_ToonSmp = sampler_state {		//サンプラーはテクスチャの色を扱う
	texture = <g_ToonTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	//トゥーンの時はClampを使うfloatは曖昧で1を若干超える時があるのでClampを使う
	AddressU = Clamp;
	AddressV = Clamp;
};





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
	Out.Pos = mul(pos, g_mW);       // ワールド変換　キャラの行列で変換する＝「ワールド座標」といいます
		Out.wPos = Out.Pos.xyz;	    // ワールド座標をピクセルシェーダへ渡す
	Out.Pos = mul(Out.Pos, g_mV);	// ビュー変換　カメラを考慮した3D座標になる＝「ビュー座標」といいます
		Out.wvPos = Out.Pos.xyz; 
	Out.Pos = mul(Out.Pos, g_mP);	// -1～1の2D空間での座標になる＝「射影座標」といいます

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

//==============================================================
// スキンメッシュ用 頂点頂点シェーダ
//==============================================================
VS_OUT SkinVS(
	float4 pos : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD0,

	float4 BlendWeights : BLENDWEIGHT, // ボーンウェイト
	float4 BlendIndices : BLENDINDICES, // ボーンインデックス
	uniform int NumBones
)
{
	VS_OUT Out = (VS_OUT)0;		// ピクセルデータに渡すデータ用構造体

	//-------------------------------------
	// スキニング
	//-------------------------------------
	// Compensate for lack of UBYTE4 on Geforce3
    int4 IndexVector = D3DCOLORtoUBYTE4(BlendIndices);

	// 配列に入れる
    float BlendWeightsArray[4] = (float[4]) BlendWeights;
    int IndexArray[4] = (int[4]) IndexVector;

	// 行列ブレンド
	float4x3 blendMat = 0;
	float LastWeight = 0.0f;

	[unroll]
	for (int iBone = 0; iBone < NumBones - 1; iBone++)
	{
		LastWeight = LastWeight + BlendWeightsArray[iBone];

		blendMat += g_mWArray[IndexArray[iBone]] * BlendWeightsArray[iBone];
	}
	LastWeight = 1.0f - LastWeight;
	blendMat += g_mWArray[IndexArray[NumBones - 1]] * LastWeight;

	// 頂点データ変換
	pos.xyz = mul(pos, (float4x3)blendMat);
	normal = mul(normal, (float3x3)blendMat);

	//-------------------------------------
	



	//-------------------------------------
	// 頂点座標を2D(射影座標系)へ変換
	//-------------------------------------
	Out.Pos = mul(pos, g_mW);       // ワールド変換　キャラの行列で変換する＝「ワールド座標」といいます
		Out.wPos = Out.Pos.xyz;	    // ワールド座標をピクセルシェーダへ渡す
	Out.Pos = mul(Out.Pos, g_mV);	// ビュー変換　カメラを考慮した3D座標になる＝「ビュー座標」といいます
		Out.wvPos = Out.Pos.xyz; 
	Out.Pos = mul(Out.Pos, g_mP);	// -1～1の2D空間での座標になる＝「射影座標」といいます

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
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//法線の正規化
	float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};

	// 法線マップから法線データを取得
	float3 normal = tex2D(g_MateNormalSmp, In.UV).xyz;
	normal = normal * 2.0 - 1.0;            // 0～1 のデータを -1～1 へ変換

											// 法線をピクセル方向に向ける
	float3 w_normal = normalize(mul(normal, mTBN));

	//---------------------------------------------------------------------
	//モデルの色
	//---------------------------------------------------------------------
	float4 texCol = tex2D(g_MateDiffuseSmp, In.UV);			//tex2Dサンプラから色を取得(テクスチャ)	  tex2D(サンプラ,取得したい座標(float2))
															//float4 modelCol = g_MateDiffuse;
	float4 modelCol = g_MateDiffuse*texCol;					//これで色をセット(テクスチャに何も入っていないときは黒で描画される)

	//---------------------------------------------------------------------
	//カメラ(目)への方向
	//---------------------------------------------------------------------
	float3 vCam = normalize(g_CamPos - In.wPos);		//In.wPosはピクセルの3D座標

	//---------------------------------------------------------------------
	//ライトの色
	//---------------------------------------------------------------------
	float3 lightCol = 0;		//全ライトの合計色(float3つ分に0を代入している)
	float3 lightSpe = 0;	//全てのライトの合計反射色



	if (g_LightEnable) {		//shaderではif文を使うと処理が重くなるので基本は使わない
	//----------------------------------------------------------------
	//平行色
	//----------------------------------------------------------------
								//拡散色(ランバード)
		float LPow = dot(-g_LightDir, w_normal);		//光の強さを-1～1で求める
		LPow = saturate(LPow);							//LPowを0～1に収める

			//テクスチャを使用して上記を解決
		float4 toonCol = tex2D(g_ToonSmp, float2(LPow, 0.5));		//tex2Dでg_ToonSmpからfloat2(LPow,0.5)の座標の色をとってくる
																	//float2(LPow,0.5)でLPowをuv座標に変換


																	//光の色を加算
																	//lightCol += g_LightColor.rgb*LPow;
		lightCol += g_LightColor.rgb*toonCol.rgb;					//toonCol.rgbは明るさを表す
																	//環境光
		lightCol += g_LightAmbient.rgb;

		//反射光(スペキュラ)
		//BlinnPhongシェーディング
		float3 vH = normalize(-g_LightDir + vCam);		//reflect関数を使わないので処理が速くなる
		float spePow = dot(vH, w_normal);
		spePow = saturate(spePow);

		spePow = pow(spePow, g_MateSpePower + 1);			//pow(a,b)は反射光のべき乗をする関数aをb分だけべき乗
															//この時g_MateSpePowerが0となる時があるので乗算が0となるとまずいので+1しておく
		lightSpe += g_LightColor.rgb*spePow;			//計算した反射光を足しこむ

	}
	else {
		lightCol = 1;
	}

	float4 diffuseCol = modelCol;
	diffuseCol.rgb *= lightCol;							//光同士は足し算光と物は掛け算(光を吸収している)

														//反射色
	float4 speCol;
	speCol.rgb = g_MateSpecular.rgb*lightSpe;
	speCol.a = 0;


	//----------------------------------------------------------------
	//出力
	//----------------------------------------------------------------
	PS_OUT Out;
	Out.Color = diffuseCol + speCol;
	//自己照明はOut.Colorで求めた色をを360度全体に発光する
	Out.Color.rgb += g_MateEmissive.rgb;
	return Out;



}



//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// テクニック
//  描画の詳細をまとめたもの。
//  つまり、使用したい頂点シェーダとピクセルシェーダを指定したり、
//  RenderStateを指定したりできる。
//
//  ※VC側からは、このテクニックを使って描画を行う感じになる
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
/*
technique Tech
{
	pass P0 {
		VertexShader = compile vs_3_0 VS();   // 頂点シェーダはVS()を使う
		PixelShader = compile ps_3_0 PS();    // ピクセルシェーダはPS()を使う
	}
}
*/

// スキンメッシュ用テクニック
VertexShader SkinVSArray[5] =
{
	compile vs_3_0 VS(),        // スキニングなし頂点シェーダ
	compile vs_3_0 SkinVS(1),   // 1ボーンでのスキニング頂点シェーダ
	compile vs_3_0 SkinVS(2),   // 2ボーンでのスキニング頂点シェーダ
	compile vs_3_0 SkinVS(3),   // 3ボーンでのスキニング頂点シェーダ
	compile vs_3_0 SkinVS(4)    // 4ボーンでのスキニング頂点シェーダ
};

technique Tech
{
	pass P0 {
		VertexShader = (SkinVSArray[g_CurNumBones]); // CurNumBonesによって頂点シェーダを選択する
		PixelShader = compile ps_3_0 PS();    // ピクセルシェーダはPS()を使う
	}
}
