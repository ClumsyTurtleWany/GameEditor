
struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
	float3 tangent : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
};

Texture2D		g_DiffuseTexture	: register(t0); // register�� �� �ٿ��ָ� �⺻������ 0�� �������Ϳ� ��.
SamplerState	g_SampleA		: register(s0); // ���ø��� �Ϸ��� ���÷��� �ʿ���

// SV_Target: ��� ������ �Ѹ� ����.
float4 PS(PixelShader_input _input) : SV_Target
{
	float4 textureColor = g_DiffuseTexture.Sample(g_SampleA, _input.t);
	
	float dist = distance(float3(0.0f, 0.0f, 0.0f), textureColor.xyz);
	if (dist < 0.5f)
	//if(textureColor.r == 0.0f && textureColor.g == 0.0f && textureColor.b == 0.0f)
	{
		discard;
	}

	return textureColor;
}