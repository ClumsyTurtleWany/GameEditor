//////////////////////////////////////
//
// PS_PointParticle_Alphatest.hlsl
//		PointParticle�� ���� ���� ���� ���� Pixel shader
//		���İ��� ���� �ؽ�ó�� ���� ó���Ѵ�.
// 
//////////////////////////////////////

//PS�ܰ迡�� �ʿ��� �͸� �Ѱ��ش�
struct PS_INPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float2 spriteUvCoord	: TEXCOORD1;
};

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);

//�� PS�ܰ迡���� �����׽�Ʈ�� �����Ѵ�.
//PS�� �Է� �μ��� GS�ܰ迡�� �þ ������ ������ �ݿ��Ѵ�.
float4 PS(PS_INPUT pIn) : SV_TARGET
{
	float4 output = g_tex0.Sample(g_sampler, pIn.spriteUvCoord) * pIn.color;
	if (output.a < 0.7f) { discard; }

	return output;
}