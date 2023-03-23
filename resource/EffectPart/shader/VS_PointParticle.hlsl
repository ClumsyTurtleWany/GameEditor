//////////////////////////////////////
//
// VS_PointParticle.hlsl
//		PointParticle을 위한 Vertex shader
//		VS에서 입력으로 받은 단일 정점을 GS셰이더에서 면으로 변경해 트랜스폼을 적용한다.
// 
//////////////////////////////////////

#define MAX_GS_VERTEXCOUNT 4

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

//VS에서 따로 작업하지 않고 그대로 GS로 입력을 넘겨준다.
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