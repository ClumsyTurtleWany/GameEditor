//////////////////////////////////////////
// 
//	PS_LineObject.hlsl
//		������ �׸� ������ ���� Pixel Shader
//		VS_LineObject.hlsl�� ¦
// 
//////////////////////////////////////////

struct PS_INPUT
{
	float4 pos		: SV_POSITION;
	float4 color	: COLOR0;
};

float4 PS(PS_INPUT pIn) : SV_TARGET
{
	float4 finalColor = pIn.color;

	return finalColor;
}