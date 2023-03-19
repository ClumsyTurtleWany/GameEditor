//////////////////////////////////////
//
// PS_PointParticle_Dualsource.hlsl
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

struct PS_OUTPUT
{
	float4 src0			: SV_TARGET0;
	float4 src1			: SV_TARGET1;
};

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);

//�� PS�ܰ迡���� �ΰ��� �ҽ��� �̿��� ���� ������ ���� �۾��� �ϴ� 
// ��� �ҽ� �÷� ������ �����Ѵ�.
//PS�� �Է� �μ��� GS�ܰ迡�� �þ ������ ������ �ݿ��Ѵ�.
PS_OUTPUT PS(PS_INPUT pIn) : SV_TARGET
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 vTexColor = g_tex0.Sample(g_sampler, pIn.spriteUvCoord);
	vTexColor *= pIn.color.a;

	//���� ������ �ƴ� ����� �����Ƿ� ��ó���� �����Ѵ�.
	if (vTexColor.r < 0.05f) { vTexColor.r = 0.0f; }
	if (vTexColor.g < 0.05f) { vTexColor.g = 0.0f; }
	if (vTexColor.b < 0.05f) { vTexColor.b = 0.0f; }

	//��ó�� �ҽ��� �ؽ�ó�� �̹����� �״�� �ݿ��Ѵ�
	//���� ��� �������� ����ũ ó���ǰ� ���ϴ� �ؽ�ó�� ����� �۾��Ѵ�.
	output.src0 = vTexColor * pIn.color;

	//���� �ؽ�ó �̹������� ���İ��� �������� ����Ѵ�.
	//������ ������ ���ο� ���Ĵ� ������ ���� ū ���� �ȴ�.
	float alpha = max(max(vTexColor.r, vTexColor.g), vTexColor.b);

	//��ó�� �ҽ��� ������ �ȼ��� ���� �������� �����ϴ� ���� ���Ͱ� �ȴ�.
	output.src1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, 1.0f);

	return output;
}
