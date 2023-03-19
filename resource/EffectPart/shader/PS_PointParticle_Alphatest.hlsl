//////////////////////////////////////
//
// PS_PointParticle_Alphatest.hlsl
//		PointParticle을 위한 알파 블렌딩 적용 Pixel shader
//		알파값을 가진 텍스처에 대해 처리한다.
// 
//////////////////////////////////////

//PS단계에서 필요한 것만 넘겨준다
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

//이 PS단계에서는 알파테스트를 수행한다.
//PS의 입력 인수는 GS단계에서 늘어난 정점의 개수를 반영한다.
float4 PS(PS_INPUT pIn) : SV_TARGET
{
	float4 output = g_tex0.Sample(g_sampler, pIn.spriteUvCoord) * pIn.color;
	if (output.a < 0.7f) { discard; }

	return output;
}