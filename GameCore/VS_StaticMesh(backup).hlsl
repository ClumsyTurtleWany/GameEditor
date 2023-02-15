// VertexShader.txt, PixelShader.txt 따로 구분하지 않고 하나의 파일에 합쳐놓아도 된다.


// HLSL (High Level Shader Language)
// 정점쉐이더란 정점 버퍼에 있는 모든 정점을 1번씩 작업한다.
// 정점 위치(SV_POSITION : System Value Position 레지스터)는 반드시 float4로 반환해야 한다.
// SV_POSITION(시멘틱 : 의미 구조) 시스템상 고정되어 있음. COLOR0 과 COLOR1 등도 있음.

// float3 p : POSITION 에서 POSITION은 Sample.hpp->CreateVertexLayout->InputElementDescs 의 요소 이름과 같아야함. 
// 고정적으로 쓰는게 아니라 그냥 변수 이름처럼 쓰지만 밖의 소스코드와 이름만 동일하면 됨.
// 정점 쉐이더랑 인풋 레이아웃이랑 다르면 만들 수가 없다.
// SV_POSITION: 출력 영역을 만들기 위한 위치 레지스터

struct VertexShader_input
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
};

struct VertexShader_output
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 과 COLOR1 밖에 없음.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) 까지 있음.
	float4 lightColor : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
	float3 light : TEXCOORD3;
};

// 상수 버퍼는 레지스터 단위로만 저장됨.
// 레지스터 단위 : float4(x, y, z, w)
// 상수 버퍼로 넘어온 애들은 전역 변수취급 됨.
cbuffer ConstantData : register(b0)
{
	matrix g_matWorld : packoffset(c0); // packoffset 을 해주지 않아도 기본적으로 c0부터 시작하나 붙여주는게 좋음
	matrix g_matView : packoffset(c4);
	matrix g_matProj : packoffset(c8);
	float4 g_light : packoffset(c12);
	//float g_nx : packoffset(c12.x);
	//float g_ny : packoffset(c12.y);
	//float g_nz : packoffset(c12.z);
	//float g_nw : packoffset(c12.w);
	//float  fTimer : packoffset(c12.z); // 이런식으로 레지스터에서 일정한 값만 등록 할 수도 있음.
}

cbuffer ConstantData_Bone : register(b1)
{
	matrix g_matBone[255];
	matrix g_matBind[255];
	matrix g_matAnim[255];
}

VertexShader_output VS(VertexShader_input _input)
{
	//VertexShader_output output;
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocal = float4(_input.p, 1.0f);

	// 주의! mul 함수는 내적(dot)으로 처리됨.
	// vWorld.x = vLocal dot c0; // 행과 행이 곱해짐.
	// vWorld.y = vLocal dot c1;
	// vWorld.z = vLocal dot c2;
	// vWorld.w = vLocal dot c3;
	// 따라서 응용프로그램 단에서 전치행렬로 만들어 보내거나, 아래와 같이 쉐이더에서 매번 전치행렬 변환작업 필요.
	// matrix matTranspose = transpose(g_matWorld); // 매번 전치행렬 변환작업하는건 비효율적이므로 가급적 응용프로그램에서 전치해서 보내줄 것.

	float4 vWorld = mul(vLocal, g_matWorld);
	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);

	//output.p = vProj;
	output.p = float4(_input.p, 1.0f);
	//output.n = _input.n;
	output.n = _input.n;
	output.c = _input.c;
	output.t = _input.t;

	output.vWorld = vWorld;

	float3 vLight = g_light.xyz; // 이런식으로 사용 가능
	float fdot = max(0.3f, dot(output.n, -vLight));// 내적값이 0이면 좋지 않음. 최소한의 값을 사용하여 윤곽이 살짝 보이게 하는것을 엠비언트 조명이라 함.
	output.lightColor = float4(fdot, fdot, fdot, 1.0f);
	output.light = vLight;

	return output;
}