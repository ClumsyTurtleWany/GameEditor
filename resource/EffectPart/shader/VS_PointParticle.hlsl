//////////////////////////////////////
//
// VS_PointParticle.hlsl
//		PointParticle�� ���� Vertex shader
//		VS���� �Է����� ���� ���� ������ GS���̴����� ������ ������ Ʈ�������� �����Ѵ�.
// 
//////////////////////////////////////

#define MAX_GS_VERTEXCOUNT 4

struct VS_INPUT
{
	float3		pos			: POSITION0;
	float3		normal		: NORMAL0;
	float4		color		: COLOR0;
	float2		uvCoord		: TEXCOORD0;
	float4		spriteRT	: SPRITERT0;
	float4x4	rot			: ROTATION;
	float3		scale		: SCALE0;
};

struct VS_OUTPUT
{
	float4		pos			: SV_POSITION;
	float3		normal		: NORMAL0;
	float4		color		: COLOR0;
	float2		uvCoord		: TEXCOORD0;
	float4		spriteRT	: SPRITERT0;
	float4x4	rot			: ROTATION;
	float3		scale		: SCALE0;
};

//VS���� ���� �۾����� �ʰ� �״�� GS�� �Է��� �Ѱ��ش�.
VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos =		float4(vIn.pos, 1.0f);
	output.normal =		vIn.normal;
	output.color =		vIn.color;
	output.uvCoord =	vIn.uvCoord;
	output.spriteRT =	vIn.spriteRT;
	output.rot =		vIn.rot;
	output.scale =		vIn.scale;

	return output;
}