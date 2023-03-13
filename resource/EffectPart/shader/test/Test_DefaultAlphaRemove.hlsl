//////////////////////////////////////
//
// Test_DefaultAlphaRemove.hlsl
//		PNCT정점 구조를 사용하는 테스트용 오브젝트 알파 제거 shader
//		cbuffer는 단계별 15개까지 사용가능
// 
//		벡터 * 행렬순으로 곱한다.(열 우선 행렬)
// 
//////////////////////////////////////

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld		: register(c0);
	matrix matView		: register(c4);
	matrix matProj		: register(c8);
};

struct VS_INPUT
{
	float3 pos			: POSITION0;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = float4(vIn.pos, 1.0f);
	output.pos = mul(output.pos, matWorld);
	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);

	output.normal = vIn.normal;
	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;

	return output;
}

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);

float4 PS(VS_OUTPUT pIn) : SV_TARGET
{
	float4 PSout = g_tex0.Sample(g_sampler, pIn.uvCoord);
	PSout.a = 1.0f;

	return PSout;
}