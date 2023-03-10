//----------------------------------------------
// Input
//----------------------------------------------
struct VertexShader_input
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
	float3 tangent : TANGENT;
};

//----------------------------------------------
// Output
//----------------------------------------------
struct VertexShader_output
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 과 COLOR1 밖에 없음.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) 까지 있음.
	float3 tangent : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
};

//----------------------------------------------
// Constant Buffer
//----------------------------------------------
// 상수 버퍼는 레지스터 단위로만 저장됨.
// 레지스터 단위 : float4(x, y, z, w)
// 상수 버퍼로 넘어온 애들은 전역 변수취급 됨.
// packoffset 을 해주지 않아도 기본적으로 c0부터 시작하나 붙여주는게 좋음
cbuffer TransformData : register(b0)
{
	matrix g_WorldTransform : packoffset(c0); 
	matrix g_InversedWorldTransform : packoffset(c4);
}

cbuffer CameraMatrixData : register(b1)
{
	matrix g_View : packoffset(c0);
	matrix g_Projection : packoffset(c4);
}

//cbuffer LightData : register(b2)
//{
//	float4 g_LightDirection : packoffset(c12);
//	float4 g_LightColor : packoffset(c13);
//}

VertexShader_output VS(VertexShader_input input)
{
	//VertexShader_output output;
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocal = float4(input.p, 1.0f);

	// 주의! mul 함수는 내적(dot)으로 처리됨.
	// vWorld.x = vLocal dot c0; // 행과 행이 곱해짐.
	// vWorld.y = vLocal dot c1;
	// vWorld.z = vLocal dot c2;
	// vWorld.w = vLocal dot c3;
	// 따라서 응용프로그램 단에서 전치행렬로 만들어 보내거나, 아래와 같이 쉐이더에서 매번 전치행렬 변환작업 필요.
	// matrix matTranspose = transpose(g_matWorld); // 매번 전치행렬 변환작업하는건 비효율적이므로 가급적 응용프로그램에서 전치해서 보내줄 것.

	/*float4x4 matWorld = g_WorldTransform;
	matWorld._14 = 0.0f;
	matWorld._24 = 0.0f;
	matWorld._34 = 0.0f;*/

	float4 vWorld = mul(vLocal, g_WorldTransform);
	float4 vView = mul(vWorld, g_View);
	float4 vProj = mul(vView, g_Projection);

	output.p = vProj;
	output.n = input.n;
	output.c = input.c;
	output.t = input.t;

	output.vWorld = vWorld;

	//float3 vLight = float3(1.0f, 1.0f, 1.0f);//g_LightColor.xyz; // 이런식으로 사용 가능
	//float fdot = max(0.3f, dot(output.n, -vLight));// 내적값이 0이면 좋지 않음. 최소한의 값을 사용하여 윤곽이 살짝 보이게 하는것을 엠비언트 조명이라 함.
	//output.lightColor = float4(fdot, fdot, fdot, 1.0f);
	//output.light = vLight;

	//output.world = g_WorldTransform;
	//output.view = g_View;
	//output.proj = g_Projection;

	return output;
}