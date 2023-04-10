// VertexShader.txt, PixelShader.txt ���� �������� �ʰ� �ϳ��� ���Ͽ� ���ĳ��Ƶ� �ȴ�.


// HLSL (High Level Shader Language)
// �������̴��� ���� ���ۿ� �ִ� ��� ������ 1���� �۾��Ѵ�.
// ���� ��ġ(SV_POSITION : System Value Position ��������)�� �ݵ�� float4�� ��ȯ�ؾ� �Ѵ�.
// SV_POSITION(�ø�ƽ : �ǹ� ����) �ý��ۻ� �����Ǿ� ����. COLOR0 �� COLOR1 � ����.

// float3 p : POSITION ���� POSITION�� Sample.hpp->CreateVertexLayout->InputElementDescs �� ��� �̸��� ���ƾ���. 
// ���������� ���°� �ƴ϶� �׳� ���� �̸�ó�� ������ ���� �ҽ��ڵ�� �̸��� �����ϸ� ��.
// ���� ���̴��� ��ǲ ���̾ƿ��̶� �ٸ��� ���� ���� ����.
// SV_POSITION: ��� ������ ����� ���� ��ġ ��������

struct VertexShader_input
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
	float3 tangent : TANGENT;
	float4 index : INDEX;
	float4 weight : WEIGHT;
};

struct VertexShader_output
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD0;
	// For Debug
	float4 world : TEXCOORD1;
	float4 view : TEXCOORD2;
	float4 proj : TEXCOORD3;
};

cbuffer TransformData : register(b0)
{
	matrix g_WorldTransform : packoffset(c0);
	matrix g_InversedWorldTransform : packoffset(c4);
}

cbuffer CameraMatrixData : register(b1)
{
	matrix g_View : packoffset(c0);
	matrix g_Projection : packoffset(c4);
}

cbuffer ConstantData_Bone : register(b2)
{
	matrix g_matBone[255];
}

VertexShader_output VS(VertexShader_input input)
{
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocal = float4(input.p, 1.0f);

	float4 vAnimation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 vAnimationNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 vAnimationTangent = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int iBone = 0; iBone < 4; iBone++)
	{
		uint iBoneIndex = input.index[iBone];
		float fWeight = input.weight[iBone];
		vAnimation += mul(vLocal, g_matBone[iBoneIndex]) * fWeight;
		vAnimationNormal += mul(float4(input.n, 1.0f), g_matBone[iBoneIndex]) * fWeight;
		vAnimationTangent += mul(float4(input.tangent, 1.0f), g_matBone[iBoneIndex]) * fWeight;
	}

	float4 vWorld = mul(vAnimation, g_WorldTransform);
	float4 vView = mul(vWorld, g_View);
	float4 vProj = mul(vView, g_Projection);

	float4 vWorldNormal = mul(vAnimationNormal, g_WorldTransform);
	float4 vViewNormal = mul(vWorldNormal, g_View);
	float4 vProjNormal = mul(vViewNormal, g_Projection);

	output.pos = vProj;
	output.depth = output.pos.zw;

	output.world = vWorld;
	output.view = vView;
	output.proj = vProj;

	return output;
}