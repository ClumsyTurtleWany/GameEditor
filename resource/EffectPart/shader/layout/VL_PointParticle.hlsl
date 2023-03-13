//////////////////////////////////////
//
// VL_PointParticle.hlsl
//		��ġ, �븻, ���� �÷�, UV��ǥ, Sprite Rect��ǥ, ȸ����, �����Ϸ��� 
//		�Է� ���̾ƿ����� ���� ������Ʈ �� Shader�� ���� ���̾ƿ��� Shader
// 
//////////////////////////////////////

struct VS_INPUT
{
	float3 pos			: POSITION0;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
	float4 spriteRT		: SPRITERT0;
	float3 rotation		: ROTATION0;
	float3 scale		: SCALE0;
};

struct VS_OUTPUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL0;
	float4 color		: COLOR0;
	float2 uvCoord		: TEXCOORD0;
};

VS_OUTPUT VSLayout(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	return output;
}