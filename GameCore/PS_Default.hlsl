// �ȼ� ���� ȣ��ȴ�.
// �ش� �ȼ��� �÷��� SV_Target �������Ϳ� ��ȯ �Ѵ�.
// Rasterizer(== Pixel) Stage ���� PixelShader ȣ�� �� Ƚ���� ����.

struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
	float4 lightColor : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
	float3 light : TEXCOORD3;
};

Texture2D		g_txTextureA	: register(t0); // register�� �� �ٿ��ָ� �⺻������ 0�� �������Ϳ� ��.
SamplerState	g_SampleA		: register(s0); // ���ø��� �Ϸ��� ���÷��� �ʿ���

// SV_Target: ��� ������ �Ѹ� ����.
float4 PS(PixelShader_input _input) : SV_Target
{
	float4 vColor = g_txTextureA.Sample(g_SampleA, _input.t);
	_input.c.a = 1.0f;
	float fDot = max(0.3f, dot(_input.n, -_input.light)); // max ������ ���ִ� ������ �����Ʈ ���� ȿ���� ���� ����.
	return vColor * _input.c; //* float4(fDot, fDot, fDot, 1.0f);
}