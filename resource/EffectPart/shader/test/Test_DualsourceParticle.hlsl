//////////////////////////////////////
//
// Test_DualsourceParticle.hlsl
//		PNCT정점 구조를 사용하는 테스트용 듀얼소스 파티클 shader
//		GS셰이더를 사용하며 VS에서 입력으로 받은 단일 정점을 
//		GS셰이더에서 면으로 변경해 트랜스폼을 적용한다.
// 
//////////////////////////////////////

#define MAX_GS_VERTEXCOUNT 4

cbuffer CBUF_COORDCONV_MATSET : register(b0)
{
	matrix matWorld		: register(c0);
	matrix matView		: register(c4);
	matrix matProj		: register(c8);
};

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

//원래의 VS 처리 구문
//VS_OUTPUT VS(VS_INPUT vIn)
//{
//	VS_OUTPUT output = (VS_OUTPUT)0;
//
//	output.pos = float4(vIn.pos, 1.0f);
//	output.pos = mul(output.pos, matWorld);
//	output.pos = mul(output.pos, matView);
//	output.pos = mul(output.pos, matProj);
//
//	output.normal = vIn.normal;
//	output.color = vIn.color;
//	output.uvCoord = vIn.uvCoord;
//
//	return output;
//}

//VS에서 따로 작업하지 않고 그대로 GS로 입력을 넘겨준다.
VS_OUTPUT VS(VS_INPUT vIn)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = float4(vIn.pos, 1.0f);
	output.normal = vIn.normal;
	output.color = vIn.color;
	output.uvCoord = vIn.uvCoord;
	output.spriteRT = vIn.spriteRT;
	output.rot = vIn.rot;
	output.scale = vIn.scale;

	return output;
}

//GS는 반환값이 없다.
//GS단계의 한번의 작업으로 늘어나는 정점개수의 상한을 지정한다.
[maxvertexcount(MAX_GS_VERTEXCOUNT)]
void GS(in point VS_OUTPUT gIn[1], inout TriangleStream<PS_INPUT> gOut)
{
	//출력인 TriangleStream은 출력 Primitive Topology가 Strip타입이다.
	//출력을 일정 단위로 끊고 싶은 경우 작업 요소 단위 별로 출력후 추가 기하요소에 대해
	//RestartStrip()함수를 호출해 연결 관계를 끊는 작업이 필요하다.

	//추가하고자 하는 정점을 새로만든다. 당연히 정점 구조를 출력형식에 맞춘다.
	// 시스템에서의 삼각형 스트립의 생성은 다음의 구조를 따른다.
	//    v1 -  v3 -  v5 
	//  /	\  /  \  /   \  ...
	// v0  - v2  - v4  - v6

	// v0 -> v1 -> v2	0번
	// v1 -> v3 -> v2	1번
	// v2 -> v3 -> v4	2번	...

	//활성화 되어 있지 않으면 추가 안함
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

		PS_INPUT newV;

		for (int i = 0; i < MAX_GS_VERTEXCOUNT; i++)
		{
			newV.pos = float4(newP[i].xyz, 1.0);
			newV.pos = mul(newV.pos, scale);
			newV.pos = mul(newV.pos, rot);
			newV.pos.xyz += gIn[0].pos.xyz;

			newV.normal = gIn[0].normal;
			newV.color = gIn[0].color;
			newV.uvCoord = gIn[0].uvCoord;
			newV.spriteUvCoord = newT[i];

			newV.pos = mul(newV.pos, matWorld);
			newV.pos = mul(newV.pos, matView);
			newV.pos = mul(newV.pos, matProj);

			gOut.Append(newV);
		}

		gOut.RestartStrip();
	}
}

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
