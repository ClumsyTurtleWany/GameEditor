//////////////////////////////////////
//
// VL_PointParticle.hlsl
//		위치, 노말, 정점 컬러, UV좌표, Sprite Rect좌표, 회전량, 스케일량을 
//		입력 레이아웃으로 쓰는 오브젝트 및 Shader를 위한 레이아웃용 Shader
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