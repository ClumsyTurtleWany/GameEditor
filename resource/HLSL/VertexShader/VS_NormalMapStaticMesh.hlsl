//----------------------------------------------
// Input
//----------------------------------------------
struct VertexShader_input
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
	float3 tangent : TANGENT;
};

//----------------------------------------------
// Output
//----------------------------------------------
struct VertexShader_output
{
	float4 p	: SV_POSITION;
	float3 n	: NORMAL;
	float4 c	: COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t	: TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
	float3 tangent : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
};

//----------------------------------------------
// Constant Buffer
//----------------------------------------------
// ��� ���۴� �������� �����θ� �����.
// �������� ���� : float4(x, y, z, w)
// ��� ���۷� �Ѿ�� �ֵ��� ���� ������� ��.
// packoffset �� ������ �ʾƵ� �⺻������ c0���� �����ϳ� �ٿ��ִ°� ����
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

VertexShader_output VS(VertexShader_input input)
{
	//VertexShader_output output;
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocal = float4(input.p, 1.0f);
	float4 vWorld = mul(vLocal, g_WorldTransform);
	float4 vView = mul(vWorld, g_View);
	float4 vProj = mul(vView, g_Projection);

	output.p = vProj;
	output.n = input.n;
	output.c = input.c;
	output.t = input.t;
	output.tangent = input.tangent;
	output.vWorld = vWorld;

	return output;
}