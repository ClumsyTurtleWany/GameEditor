//////////////////////////////////////
//
// GS_PointParticle.hlsl
//		PointParticle�� ���� Gemometry Shader
//		VS���� �Է����� ���� ���� ������ GS���̴����� ������ ������ Ʈ�������� �����Ѵ�.
// 
//////////////////////////////////////

#define MAX_GS_VERTEXCOUNT 4

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld		: register(c0);
	matrix matView		: register(c4);
	matrix matProj		: register(c8);
};

cbuffer CBUF_BILLBOARD_MAT : register(b1)
{
	matrix matBillboard		: register(c0);
};

struct GS_INPUT
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
struct GS_OUTPUT
{
	float4 pos				: SV_POSITION;
	float3 normal			: NORMAL0;
	float4 color			: COLOR0;
	float2 uvCoord			: TEXCOORD0;
	float2 spriteUvCoord	: TEXCOORD1;
};

//GS�� ��ȯ���� ����.
//GS�ܰ��� �ѹ��� �۾����� �þ�� ���������� ������ �����Ѵ�.
[maxvertexcount(MAX_GS_VERTEXCOUNT)]
void GS(in point GS_INPUT gIn[1], inout TriangleStream<GS_OUTPUT> gOut)
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

		GS_OUTPUT newV;

		for (int i = 0; i < MAX_GS_VERTEXCOUNT; i++)
		{
			newV.pos = float4(newP[i].xyz, 1.0);
			newV.pos = mul(newV.pos, scale);
			newV.pos = mul(newV.pos, rot);
			newV.pos.xyz = newV.pos.xyz * matWorld._11;
			newV.pos = mul(newV.pos, matBillboard);
			newV.pos.xyz += gIn[0].pos.xyz;

			newV.normal = gIn[0].normal;
			newV.color = gIn[0].color;
			newV.uvCoord = gIn[0].uvCoord;
			newV.spriteUvCoord = newT[i];

			newV.pos = mul(newV.pos, matView);
			newV.pos = mul(newV.pos, matProj);

			gOut.Append(newV);
		}

		gOut.RestartStrip();
	}
}