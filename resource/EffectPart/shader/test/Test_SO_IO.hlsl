//////////////////////////////////////
//
// Test_SO_IO.hlsl
//		PNCT정점 구조를 사용하는 테스트용 SO 입출력 shader
//		
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

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);


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

[maxvertexcount(9)]
void GS(in triangle VS_OUTPUT gIn[3], inout TriangleStream<VS_OUTPUT> gOut)
{
	VS_OUTPUT center = (VS_OUTPUT)0;

	center.pos = float4((gIn[0].pos + gIn[1].pos + gIn[2].pos) / 3.0f, 1.0f);
	center.normal = float3((gIn[0].normal + gIn[1].normal + gIn[2].normal) / 3.0f);
	center.color = float4((gIn[0].color.rgb + gIn[1].color.rgb + gIn[2].color.rgb) / 3.0f, 1.0f);
	center.uvCoord = float2((gIn[0].uvCoord + gIn[1].uvCoord + gIn[2].uvCoord) / 3.0f);

	for (int i = 0; i < 3; i++)
	{
		gOut.append(gIn[i]);
		gOut.append(gIn[(i + 1) % 3]);
		gOut.append(center);

		gOut.RestartStrip();
	}
}


float4 PS(VS_OUTPUT pIn) : SV_TARGET
{
	float4 PSout = {0.5f, 0.0f, 1.0f, 1.0f};

	return PSout;
}