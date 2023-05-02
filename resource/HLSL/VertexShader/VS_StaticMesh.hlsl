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
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 과 COLOR1 밖에 없음.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) 까지 있음.
	float3 tangent : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
};

//----------------------------------------------
// Constant Buffer
//----------------------------------------------
// 상수 버퍼는 레지스터 단위로만 저장됨.
// 레지스터 단위 : float4(x, y, z, w)
// 상수 버퍼로 넘어온 애들은 전역 변수취급 됨.
// packoffset 을 해주지 않아도 기본적으로 c0부터 시작하나 붙여주는게 좋음
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
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocalPos = float4(input.p, 1.0f);
	float4 vWorldPos = mul(vLocalPos, g_WorldTransform);
	float3 vWorldNormal = mul(input.n, (float3x3)g_WorldTransform);
	float3 vWorldTangent = mul(input.tangent, (float3x3)g_WorldTransform);

	float4 vViewPos = mul(vWorldPos, g_View);
	float4 vProjPos = mul(vViewPos, g_Projection);

	output.p = vProjPos;
	output.n = normalize(vWorldNormal).xyz;
	output.c = input.c;
	output.t = input.t;
	output.tangent = normalize(vWorldTangent).xyz;
	output.vWorld = vWorldPos;

	return output;
}