//////////////////////////////////////
//
// Test_DefaultParticle.hlsl
//		PNCT���� ������ ����ϴ� �׽�Ʈ�� ��ƼŬ shader
//		GS���̴��� ����ϸ� VS���� �Է����� ���� ���� ������ 
//		GS���̴����� ������ ������ Ʈ�������� �����Ѵ�.
// 
//////////////////////////////////////

#define MAX_GS_VERTEXCOUNT 4

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
	float4 spriteRT		: SPRITERT0;
	float3 rot			: ROTATION0;
	float3 scale		: SCALE0;
};

struct VS_OUTPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
	float4 spriteRT		: SPRITERT0;
	float3 rot			: ROTATION0;
	float3 scale		: SCALE0;
};

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

//������ VS ó�� ����
//VS_OUTPUT VS(VS_INPUT vIn)
//{
//	VS_OUTPUT output = (VS_OUTPUT)0;
//
//	output.pos = float4(vIn.pos, 1.0f);
//	output.pos = mul(output.pos, matWorld);
//	output.pos = mul(output.pos, matView);
//	output.pos = mul(output.pos, matProj);
//
//	output.normal = vIn.normal;
//	output.color = vIn.color;
//	output.uvCoord = vIn.uvCoord;
//
//	return output;
//}

//VS���� ���� �۾����� �ʰ� �״�� GS�� �Է��� �Ѱ��ش�.
VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = float4(vIn.pos, 1.0f);
	output.normal = vIn.normal;
	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;
	output.spriteRT = vIn.spriteRT;
	output.rot = vIn.rot;
	output.scale = vIn.scale;

	return output;
}

//GS�� ��ȯ���� ����.
//GS�ܰ��� �ѹ��� �۾����� �þ�� ���������� ������ �����Ѵ�.
[maxvertexcount(MAX_GS_VERTEXCOUNT)]
void GS(in point VS_OUTPUT gIn[1], inout TriangleStream<PS_INPUT> gOut)
{
	//����� TriangleStream�� ��� Primitive Topology�� StripŸ���̴�.
	//����� ���� ������ ���� ���� ��� �۾� ��� ���� ���� ����� �߰� ���Ͽ�ҿ� ����
	//RestartStrip()�Լ��� ȣ���� ���� ���踦 ���� �۾��� �ʿ��ϴ�.

	//�߰��ϰ��� �ϴ� ������ ���θ����. �翬�� ���� ������ ������Ŀ� �����.
	// �ý��ۿ����� �ﰢ�� ��Ʈ���� ������ ������ ������ ������.
	//    v1 -  v3 -  v5 
	//  /	\  /  \  /   \  ...
	// v0  - v2  - v4  - v6

	// v0 -> v1 -> v2	0��
	// v1 -> v3 -> v2	1��
	// v2 -> v3 -> v4	2��	...

	//Ȱ��ȭ �Ǿ� ���� ������ �߰� ����
	if (gIn[0].rot.z > 0.0f)
	{
		float3 newP[4] = {
			{-0.5f, 0.5f, 0.0f},
			{0.5f, 0.5f, 0.0f},
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f}
		};

		float2 newT[4] = {
			{gIn[0].spriteRT.x, gIn[0].spriteRT.y},
			{gIn[0].spriteRT.z, gIn[0].spriteRT.y},
			{gIn[0].spriteRT.x, gIn[0].spriteRT.w},
			{gIn[0].spriteRT.z, gIn[0].spriteRT.w}
		};

		matrix rot = {
			gIn[0].rot.x, gIn[0].rot.y, 0.0f, 0.0f,
			-gIn[0].rot.y, gIn[0].rot.x, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		matrix scale = {
			gIn[0].scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, gIn[0].scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		PS_INPUT newV;

		for (int i = 0; i < MAX_GS_VERTEXCOUNT; i++)
		{
			newV.pos = float4(newP[i].xyz, 1.0);
			newV.pos = mul(newV.pos, scale);
			newV.pos = mul(newV.pos, rot);
			newV.pos.xyz += gIn[0].pos.xyz;

			newV.normal = gIn[0].normal;
			newV.color = gIn[0].color;
			newV.uvCoord = gIn[0].uvCoord;
			newV.spriteUvCoord = newT[i];

			newV.pos = mul(newV.pos, matWorld);
			newV.pos = mul(newV.pos, matView);
			newV.pos = mul(newV.pos, matProj);

			gOut.Append(newV);
		}

		gOut.RestartStrip();
	}
}

//�� PS�ܰ迡���� ���� ������ �ݿ��Ѵ�.
//PS�� �Է� �μ��� GS�ܰ迡�� �þ ������ ������ �ݿ��Ѵ�.
//���ε� ���� ���� ���̴� �ؽ�ó ������ ���ø��� ���� (0, 0, 0, 0)�� ������ �ȴ�.
float4 PS(PS_INPUT pIn) : SV_TARGET
{
	float4 output = g_tex0.Sample(g_sampler, pIn.spriteUvCoord) * pIn.color;
	
	return output;
}
