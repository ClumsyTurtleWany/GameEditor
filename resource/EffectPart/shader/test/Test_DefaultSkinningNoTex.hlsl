//////////////////////////////////////
//
// Test_DefaultSkinning.hlsl
//		PNCT정점 구조를 사용하는 테스트용 오브젝트 shader
//		cbuffer는 단계별 15개까지 사용가능
// 
//		벡터 * 행렬순으로 곱한다.(열 우선 행렬)
// 
//////////////////////////////////////

#define MAX_NUMBER_OF_MATRIX 256

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld		: register(c0);
	matrix matView		: register(c4);
	matrix matProj		: register(c8);
};

cbuffer CBuf_AnimMat : register(b1)
{
	matrix g_animMat : packoffset(c0);
};

struct VS_INPUT
{
	float3 pos			: POSITION0;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
	float4 boneIndex	: INDEX0;
	float4 weight		: WEIGHT0;
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

	float4 vPos = float4(vIn.pos, 1.0f);
	float4 vNewPos = 0;
	float3 vNewNormal = 0;

	////가중치의 합산이 1이어야한다.
	//for (int idx = 0; idx < 4; idx++)
	//{
	//	vNewPos += mul(vPos, g_skinningMat[vIn.boneIndex[idx]]) * vIn.weight[idx];
	//	vNewNormal += mul(vIn.n, g_skinningMat[vIn.boneIndex[idx]]) * vIn.weight[idx];
	//}

	vNewPos = mul(vPos, g_animMat);
	vNewNormal = mul(vIn.normal, g_animMat);

	output.pos = vNewPos;
	output.pos = mul(output.pos, matWorld);
	output.pos = mul(output.pos, matView);
	output.pos = mul(output.pos, matProj);

	output.normal = vNewNormal;
	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;

	return output;
}

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);

float4 PS(VS_OUTPUT pIn) : SV_TARGET
{
	float4 PSout = pIn.color;
	//float4 PSout = g_tex0.Sample(g_sampler, pIn.uvCoord) * pIn.color;

	return PSout;
}