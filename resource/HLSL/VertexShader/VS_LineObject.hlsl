//////////////////////////////////////////
// 
//	VS_LineObject.hlsl
//		������ �׸� ������ ���� Vertex Shader
//		PS_LineObject.hlsl�� ¦
// 
//////////////////////////////////////////

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld		: register(c0);
	matrix matView		: register(c4);
	matrix matProj		: register(c8);
};

struct VS_INPUT
{
	float3 pos		: POSITION0;
	float4 color	: COLOR0;
};

struct VS_OUTPUT
{
	float4 pos		: SV_POSITION0;
	float4 color	: COLOR0;
};

VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT vOut = (VS_OUTPUT)0;

	vOut.pos = float4(vIn.pos, 1.0f);
	vOut.pos = mul(vOut.pos, matWorld);
	vOut.pos = mul(vOut.pos, matView);
	vOut.pos = mul(vOut.pos, matProj);

	vOut.color = vIn.color;

	return vOut;
}