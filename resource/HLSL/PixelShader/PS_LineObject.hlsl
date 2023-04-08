//////////////////////////////////////////
// 
//	PS_LineObject.hlsl
//		선으로 그릴 도형을 위한 Pixel Shader
//		VS_LineObject.hlsl과 짝
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