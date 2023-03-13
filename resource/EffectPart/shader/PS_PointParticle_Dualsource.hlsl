//////////////////////////////////////
//
// PS_PointParticle_Dualsource.hlsl
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

struct PS_OUTPUT
{
	float4 src0			: SV_TARGET0;
	float4 src1			: SV_TARGET1;
};

Texture2D g_tex0 : register(t0);
Texture2D g_tex1 : register(t1);

SamplerState g_sampler : register(s0);

//이 PS단계에서는 두개의 소스를 이용해 알파 블렌딩과 같은 작업을 하는 
// 듀얼 소스 컬러 블렌딩을 수행한다.
//PS의 입력 인수는 GS단계에서 늘어난 정점의 개수를 반영한다.
PS_OUTPUT PS(PS_INPUT pIn) : SV_TARGET
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	float4 vTexColor = g_tex0.Sample(g_sampler, pIn.spriteUvCoord);
	vTexColor *= pIn.color.a;

	//완전 검정이 아닌 배경이 있으므로 전처리를 수행한다.
	if (vTexColor.r < 0.05f) { vTexColor.r = 0.0f; }
	if (vTexColor.g < 0.05f) { vTexColor.g = 0.0f; }
	if (vTexColor.b < 0.05f) { vTexColor.b = 0.0f; }

	//선처리 소스는 텍스처의 이미지를 그대로 반영한다
	//알파 대신 검은색이 마스크 처리되게 끔하는 텍스처를 사용해 작업한다.
	output.src0 = vTexColor * pIn.color;

	//검정 텍스처 이미지에서 알파값을 색상으로 대신한다.
	//색상이 있으면 새로운 알파는 색상중 가장 큰 값이 된다.
	float alpha = max(max(vTexColor.r, vTexColor.g), vTexColor.b);

	//후처리 소스는 검은색 픽셀을 완전 투명으로 간주하는 블렌딩 팩터가 된다.
	output.src1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, 1.0f);

	return output;
}
