struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

cbuffer TextureIndex : register(b0)
{
	int g_TextureIndex;
	int g_Empty0;
	int g_Empty1;
	int g_Empty2;
};

Texture2DArray	g_txTextureA	: register(t0);
SamplerState	g_SampleA		: register(s0);

float4 PS(PixelShader_input input) : SV_Target
{
	float4 vColor = g_txTextureA.Sample(g_SampleA, float3(input.t, g_TextureIndex));
	return vColor * input.c;
}