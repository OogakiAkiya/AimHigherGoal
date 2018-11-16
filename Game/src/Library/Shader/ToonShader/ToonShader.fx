//������������������������������������������������������������������������
//
// �O���[�o���萔
//�@��VC������A�D���Ȓl���Z�b�g�ł���B��̏ꏊ
//
//������������������������������������������������������������������������

//-----------------------------------
// �ϊ��s��
//-----------------------------------
float4x4 g_mW;		// ���[���h�s��@�܂�L�����̍s��
float4x4 g_mV;		// �r���[�s��
float4x4 g_mP;		// �ˉe�s��

// �X�L�����b�V���p
static const int MAX_MATRICES = 26; // �ő�{�[����
float4x3 g_mWArray[MAX_MATRICES]; // �{�[���s��z��
int g_CurNumBones = 2; // �e���{�[����

//-----------------------------------
// �}�e���A���֌W
//-----------------------------------
float4	g_MateDiffuse = float4(1,1,1,1);	// �}�e���A���̊g�U(Diffuse)�F
float4	g_MateSpecular = float4(1,1,1,1);	// �}�e���A���̔���(Specular)�F �� ���������̂悤�ɔ��˂���Ƃ��̐F
float	g_MateSpePower = 50;				// �}�e���A���̔���(Specular)�̗�
float4  g_MateEmissive = float4(0,0,0,0);   // �}�e���A���̎��Ȕ����F�@���̃T���v���ł͖��g�p�ł�
// ���}�e���A����Ambient�͎g��Ȃ�

//-----------------------------------
// �e�N�X�`��
//-----------------------------------
texture g_MateDiffuseTex;         // �}�e���A���̃e�N�X�`��
// MateDiffuseTex�p�̃T���v��
// �����ۂɃe�N�X�`������F���擾����ɂ́A�������g��
sampler g_MateDiffuseSmp = sampler_state
{
	Texture = <g_MateDiffuseTex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

texture g_MateNormalTex; // �}�e���A���̃e�N�X�`��
// MateDiffuseTex�p�̃T���v��
// �����ۂɃe�N�X�`������F���擾����ɂ́A�������g��
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
// ���C�g�֌W
//-----------------------------------
// ���s����
int    g_LightEnable = 1;
float3 g_LightDir = float3(0 ,-1, 0);	    // ���s���̕���
float4 g_LightColor = float4(1, 1, 1, 1); // ���s����Diffuse�F

// ����
float4 g_LightAmbient;                    // �����̐F

//-----------------------------------
// �J�����p
//-----------------------------------
float3 g_CamPos; // �J�������W


//�e�N�X�`��
//�g�D�[��
texture g_ToonTex;						//�A�e�p�̃e�N�X�`��(ModelShader.h�ɒǉ�����܂Ńe�N�X�`�����Z�b�g����Ă��Ȃ�)
sampler g_ToonSmp = sampler_state {		//�T���v���[�̓e�N�X�`���̐F������
	texture = <g_ToonTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	//�g�D�[���̎���Clamp���g��float�͞B����1���኱�����鎞������̂�Clamp���g��
	AddressU = Clamp;
	AddressV = Clamp;
};





//������������������������������������������������������������������������
//
// �������璸�_�V�F�[�_
//
//������������������������������������������������������������������������

//==============================================================
// ���_�V�F�[�_����s�N�Z���V�F�[�_�ɓn���f�[�^�p�\����
//==============================================================
struct VS_OUT
{
	float4 Pos		: POSITION;		// 2D���W
	float2 UV	    : TEXCOORD0;	// �e�N�X�`��UV
	
	float3 wPos		: TEXCOORD1;	// 3D���W
	float3 wN		: TEXCOORD2;	// �@��

	float3 wT       : TEXCOORD3;
	float3 wB       : TEXCOORD4;
	float3 wvPos    : TEXCOORD5;
};

//==============================================================
// ���_�V�F�[�_
//	�E���_���P�����Ă���̂ŁA������������s�N�Z���V�F�[�_�ɓn���B
//==============================================================
VS_OUT VS (
	float4 pos : POSITION,				// ���W(���f���̒��_���W�����̂܂ܗ���@�܂�S���L�����̍s��Ƃ��l������Ă܂��񁁁u���[�J�����W�v�Ƃ����܂�)
	float3 normal : NORMAL,             // �@��(���f���̖@�������̂܂ܗ���@�܂�S���L�����̍s��Ƃ��l������Ă܂���)
	float2 uv : TEXCOORD0			// �e�N�X�`��UV
) {
	VS_OUT Out = (VS_OUT)0;		// �s�N�Z���f�[�^�ɓn���f�[�^�p�\����

	//-------------------------------------
	// ���_���W��2D(�ˉe���W�n)�֕ϊ�
	//-------------------------------------
	Out.Pos = mul(pos, g_mW);       // ���[���h�ϊ��@�L�����̍s��ŕϊ����遁�u���[���h���W�v�Ƃ����܂�
		Out.wPos = Out.Pos.xyz;	    // ���[���h���W���s�N�Z���V�F�[�_�֓n��
	Out.Pos = mul(Out.Pos, g_mV);	// �r���[�ϊ��@�J�������l������3D���W�ɂȂ遁�u�r���[���W�v�Ƃ����܂�
		Out.wvPos = Out.Pos.xyz; 
	Out.Pos = mul(Out.Pos, g_mP);	// -1�`1��2D��Ԃł̍��W�ɂȂ遁�u�ˉe���W�v�Ƃ����܂�

	// �e�N�X�`��UV���W�����̂܂܃s�N�Z���V�F�[�_��(������ϐ�����΁AUV�X�N���[���Ƃ����ł���)
	Out.UV = uv;

	//--------------------------------------
	// �ȈՔ� �@���}�b�v�p�x�N�g������
	//--------------------------------------
	Out.wN = normalize(mul(normal, (float3x3) g_mW)); // ���[�J���@�������[���h�@���ɕϊ�
	// X�������߂邽�߁A[0,1,0]��Z��(wN)�̊O�ς��v�Z���邪�AwN���^��(0, 1, 0)�̏ꍇ�͌��ʂ�[0,0,0]�ɂȂ��Ă��܂��B
	// �Ȃ̂ŁAwN��[0,1,0]�̋߂��ꍇ�́A�ʂ̕����ƊO�ς����邱�ƂőΏ��B
	// ��if���A�ȉ��̂悤�ȏ������̂ق����p�t�H�[�}���X���ǂ�
	float3 vUp = abs(dot(Out.wN, float3(0, 1, 0))) <= 0.999999 ? float3(0, 1, 0) : float3(0, 0, -1);    // wN��[0,1,0]�Ɍ���Ȃ��߂��ꍇ�́A(0,0,-1)�ŊO�ς���B
	Out.wT = normalize(cross(vUp, Out.wN));
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}

//==============================================================
// �X�L�����b�V���p ���_���_�V�F�[�_
//==============================================================
VS_OUT SkinVS(
	float4 pos : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD0,

	float4 BlendWeights : BLENDWEIGHT, // �{�[���E�F�C�g
	float4 BlendIndices : BLENDINDICES, // �{�[���C���f�b�N�X
	uniform int NumBones
)
{
	VS_OUT Out = (VS_OUT)0;		// �s�N�Z���f�[�^�ɓn���f�[�^�p�\����

	//-------------------------------------
	// �X�L�j���O
	//-------------------------------------
	// Compensate for lack of UBYTE4 on Geforce3
    int4 IndexVector = D3DCOLORtoUBYTE4(BlendIndices);

	// �z��ɓ����
    float BlendWeightsArray[4] = (float[4]) BlendWeights;
    int IndexArray[4] = (int[4]) IndexVector;

	// �s��u�����h
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

	// ���_�f�[�^�ϊ�
	pos.xyz = mul(pos, (float4x3)blendMat);
	normal = mul(normal, (float3x3)blendMat);

	//-------------------------------------
	



	//-------------------------------------
	// ���_���W��2D(�ˉe���W�n)�֕ϊ�
	//-------------------------------------
	Out.Pos = mul(pos, g_mW);       // ���[���h�ϊ��@�L�����̍s��ŕϊ����遁�u���[���h���W�v�Ƃ����܂�
		Out.wPos = Out.Pos.xyz;	    // ���[���h���W���s�N�Z���V�F�[�_�֓n��
	Out.Pos = mul(Out.Pos, g_mV);	// �r���[�ϊ��@�J�������l������3D���W�ɂȂ遁�u�r���[���W�v�Ƃ����܂�
		Out.wvPos = Out.Pos.xyz; 
	Out.Pos = mul(Out.Pos, g_mP);	// -1�`1��2D��Ԃł̍��W�ɂȂ遁�u�ˉe���W�v�Ƃ����܂�

	// �e�N�X�`��UV���W�����̂܂܃s�N�Z���V�F�[�_��(������ϐ�����΁AUV�X�N���[���Ƃ����ł���)
	Out.UV = uv;

	//--------------------------------------
	// �ȈՔ� �@���}�b�v�p�x�N�g������
	//--------------------------------------
	Out.wN = normalize(mul(normal, (float3x3) g_mW)); // ���[�J���@�������[���h�@���ɕϊ�
	// X�������߂邽�߁A[0,1,0]��Z��(wN)�̊O�ς��v�Z���邪�AwN���^��(0, 1, 0)�̏ꍇ�͌��ʂ�[0,0,0]�ɂȂ��Ă��܂��B
	// �Ȃ̂ŁAwN��[0,1,0]�̋߂��ꍇ�́A�ʂ̕����ƊO�ς����邱�ƂőΏ��B
	// ��if���A�ȉ��̂悤�ȏ������̂ق����p�t�H�[�}���X���ǂ�
	float3 vUp = abs(dot(Out.wN, float3(0, 1, 0))) <= 0.999999 ? float3(0, 1, 0) : float3(0, 0, -1);    // wN��[0,1,0]�Ɍ���Ȃ��߂��ꍇ�́A(0,0,-1)�ŊO�ς���B
	Out.wT = normalize(cross(vUp, Out.wN));
	Out.wB = normalize(cross(Out.wN, Out.wT));

	return Out;
}

//������������������������������������������������������������������������
//
// ��������s�N�Z���V�F�[�_
//
//������������������������������������������������������������������������

//==================================================================
// �s�N�Z���V�F�[�_����o�͂���f�[�^�p�\����
//==================================================================
struct PS_OUT
{
	float4 Color : COLOR0;  // �o�͐F(RenderTarget��0�Ԗڂɏo�͂���)
};

// �s�N�Z���V�F�[�_
//	�E�s�N�Z���̐F�����肵�A���̐F��Ԃ�(R,G,B,A��float4�^)
PS_OUT PS(VS_OUT In)
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//�@���̐��K��
	float3x3 mTBN = {
		normalize(In.wT),
		normalize(In.wB),
		normalize(In.wN)
	};

	// �@���}�b�v����@���f�[�^���擾
	float3 normal = tex2D(g_MateNormalSmp, In.UV).xyz;
	normal = normal * 2.0 - 1.0;            // 0�`1 �̃f�[�^�� -1�`1 �֕ϊ�

											// �@�����s�N�Z�������Ɍ�����
	float3 w_normal = normalize(mul(normal, mTBN));

	//---------------------------------------------------------------------
	//���f���̐F
	//---------------------------------------------------------------------
	float4 texCol = tex2D(g_MateDiffuseSmp, In.UV);			//tex2D�T���v������F���擾(�e�N�X�`��)	  tex2D(�T���v��,�擾���������W(float2))
															//float4 modelCol = g_MateDiffuse;
	float4 modelCol = g_MateDiffuse*texCol;					//����ŐF���Z�b�g(�e�N�X�`���ɉ��������Ă��Ȃ��Ƃ��͍��ŕ`�悳���)

	//---------------------------------------------------------------------
	//�J����(��)�ւ̕���
	//---------------------------------------------------------------------
	float3 vCam = normalize(g_CamPos - In.wPos);		//In.wPos�̓s�N�Z����3D���W

	//---------------------------------------------------------------------
	//���C�g�̐F
	//---------------------------------------------------------------------
	float3 lightCol = 0;		//�S���C�g�̍��v�F(float3����0�������Ă���)
	float3 lightSpe = 0;	//�S�Ẵ��C�g�̍��v���ːF



	if (g_LightEnable) {		//shader�ł�if�����g���Ə������d���Ȃ�̂Ŋ�{�͎g��Ȃ�
	//----------------------------------------------------------------
	//���s�F
	//----------------------------------------------------------------
								//�g�U�F(�����o�[�h)
		float LPow = dot(-g_LightDir, w_normal);		//���̋�����-1�`1�ŋ��߂�
		LPow = saturate(LPow);							//LPow��0�`1�Ɏ��߂�

			//�e�N�X�`�����g�p���ď�L������
		float4 toonCol = tex2D(g_ToonSmp, float2(LPow, 0.5));		//tex2D��g_ToonSmp����float2(LPow,0.5)�̍��W�̐F���Ƃ��Ă���
																	//float2(LPow,0.5)��LPow��uv���W�ɕϊ�


																	//���̐F�����Z
																	//lightCol += g_LightColor.rgb*LPow;
		lightCol += g_LightColor.rgb*toonCol.rgb;					//toonCol.rgb�͖��邳��\��
																	//����
		lightCol += g_LightAmbient.rgb;

		//���ˌ�(�X�y�L����)
		//BlinnPhong�V�F�[�f�B���O
		float3 vH = normalize(-g_LightDir + vCam);		//reflect�֐����g��Ȃ��̂ŏ����������Ȃ�
		float spePow = dot(vH, w_normal);
		spePow = saturate(spePow);

		spePow = pow(spePow, g_MateSpePower + 1);			//pow(a,b)�͔��ˌ��ׂ̂��������֐�a��b�������ׂ���
															//���̎�g_MateSpePower��0�ƂȂ鎞������̂ŏ�Z��0�ƂȂ�Ƃ܂����̂�+1���Ă���
		lightSpe += g_LightColor.rgb*spePow;			//�v�Z�������ˌ��𑫂�����

	}
	else {
		lightCol = 1;
	}

	float4 diffuseCol = modelCol;
	diffuseCol.rgb *= lightCol;							//�����m�͑����Z���ƕ��͊|���Z(�����z�����Ă���)

														//���ːF
	float4 speCol;
	speCol.rgb = g_MateSpecular.rgb*lightSpe;
	speCol.a = 0;


	//----------------------------------------------------------------
	//�o��
	//----------------------------------------------------------------
	PS_OUT Out;
	Out.Color = diffuseCol + speCol;
	//���ȏƖ���Out.Color�ŋ��߂��F����360�x�S�̂ɔ�������
	Out.Color.rgb += g_MateEmissive.rgb;
	return Out;



}



//������������������������������������������������������������������������
// �e�N�j�b�N
//  �`��̏ڍׂ��܂Ƃ߂����́B
//  �܂�A�g�p���������_�V�F�[�_�ƃs�N�Z���V�F�[�_���w�肵����A
//  RenderState���w�肵����ł���B
//
//  ��VC������́A���̃e�N�j�b�N���g���ĕ`����s�������ɂȂ�
//������������������������������������������������������������������������
/*
technique Tech
{
	pass P0 {
		VertexShader = compile vs_3_0 VS();   // ���_�V�F�[�_��VS()���g��
		PixelShader = compile ps_3_0 PS();    // �s�N�Z���V�F�[�_��PS()���g��
	}
}
*/

// �X�L�����b�V���p�e�N�j�b�N
VertexShader SkinVSArray[5] =
{
	compile vs_3_0 VS(),        // �X�L�j���O�Ȃ����_�V�F�[�_
	compile vs_3_0 SkinVS(1),   // 1�{�[���ł̃X�L�j���O���_�V�F�[�_
	compile vs_3_0 SkinVS(2),   // 2�{�[���ł̃X�L�j���O���_�V�F�[�_
	compile vs_3_0 SkinVS(3),   // 3�{�[���ł̃X�L�j���O���_�V�F�[�_
	compile vs_3_0 SkinVS(4)    // 4�{�[���ł̃X�L�j���O���_�V�F�[�_
};

technique Tech
{
	pass P0 {
		VertexShader = (SkinVSArray[g_CurNumBones]); // CurNumBones�ɂ���Ē��_�V�F�[�_��I������
		PixelShader = compile ps_3_0 PS();    // �s�N�Z���V�F�[�_��PS()���g��
	}
}
