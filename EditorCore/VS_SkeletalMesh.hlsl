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
	float3 tangent : TANGENT;
	float4 index : INDEX;
	float4 weight : WEIGHT;
};

struct VertexShader_output
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 과 COLOR1 밖에 없음.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) 까지 있음.
	float3 tangent : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
};

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

cbuffer ConstantData_Bone : register(b2)
{
	matrix g_matBone[255];
}

VertexShader_output VS(VertexShader_input input)
{
	VertexShader_output output = (VertexShader_output)0;

	float4 vLocal = float4(input.p, 1.0f);

	float4 vAnimation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 vAnimationNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 vAnimationTangent = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int iBone = 0; iBone < 4; iBone++)
	{
		uint iBoneIndex = input.index[iBone];
		float fWeight = input.weight[iBone];
		vAnimation += mul(vLocal, g_matBone[iBoneIndex]) * fWeight;
		vAnimationNormal += mul(input.n, g_matBone[iBoneIndex]) * fWeight;
		vAnimationTangent += mul(input.tangent, g_matBone[iBoneIndex]) * fWeight;
	}

	float4 vWorld = mul(vAnimation, g_WorldTransform);
	float4 vView = mul(vWorld, g_View);
	float4 vProj = mul(vView, g_Projection);

	output.p = vProj;
	output.n = vAnimationNormal;
	output.c = input.c;
	output.t = input.t;
	output.tangent = vAnimationTangent;
	output.vWorld = vWorld;

	return output;
}