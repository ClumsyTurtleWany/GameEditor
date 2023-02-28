// 픽셀 마다 호출된다.
// 해당 픽셀의 컬러를 SV_Target 레지스터에 반환 한다.
// Rasterizer(== Pixel) Stage 에서 PixelShader 호출 할 횟수를 결정.

struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 과 COLOR1 밖에 없음.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) 까지 있음.
	//float4 lightColor : TEXCOORD1;
	float4 vWorld : TEXCOORD1;
	//float3 light : TEXCOORD3;
};

Texture2D		g_txTextureA	: register(t0); // register를 안 붙여주면 기본적으로 0번 레지스터에 들어감.
SamplerState	g_SampleA		: register(s0); // 샘플링을 하려면 샘플러가 필요함

// SV_Target: 출력 영역에 뿌릴 색상.
float4 PS(PixelShader_input _input) : SV_Target
{
	float4 vColor = g_txTextureA.Sample(g_SampleA, _input.t);
	_input.c.a = 1.0f;
	float3 vLight = float3(0.0f, -1.0f, 0.0f);
	float fDot = max(0.3f, dot(_input.n, -vLight)); // max 값으로 해주는 이유는 엠비언트 조명 효과를 내기 위함.
	return vColor * _input.c; //* float4(fDot, fDot, fDot, 1.0f);
}