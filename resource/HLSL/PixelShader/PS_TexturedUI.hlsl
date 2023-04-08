struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0;
	float2 t : TEXCOORD0;
};

Texture2D		g_txTextureA	: register(t0);
SamplerState	g_SampleA		: register(s0);

float4 PS(PixelShader_input input) : SV_Target
{
	float4 vColor = g_txTextureA.Sample(g_SampleA, input.t);
	return vColor * input.c;
}