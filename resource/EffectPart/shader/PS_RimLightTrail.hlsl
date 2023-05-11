#define EPSILON (float)0.0001
#define MAX_DIRECTIONAL_LIGHT_CNT (int)10
#define MAX_POINT_LIGHT_CNT (int)10
#define MAX_SPOT_LIGHT_CNT (int)10

struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 과 COLOR1 밖에 없음.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) 까지 있음.
	float3 tangent : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
};

cbuffer DirectionLightData : register(b0)
{
	float4 g_DirectionLightColor[MAX_DIRECTIONAL_LIGHT_CNT];
	float4 g_DirectionLightPos[MAX_DIRECTIONAL_LIGHT_CNT];
	float4 g_DirectionLightDir[MAX_DIRECTIONAL_LIGHT_CNT];
	float4x4 g_DirectionalLightView[MAX_DIRECTIONAL_LIGHT_CNT];
	int  g_DirectionLightCnt;
	int g_DirectionLightShadowTextureSize;
	int g_DirectionalDummy1;
	int g_DirectionalDummy2;
}

cbuffer PointLightData : register(b1)
{
	float4 g_PointLightColor[MAX_POINT_LIGHT_CNT];
	float4 g_PointLightPos[MAX_POINT_LIGHT_CNT];
	float4 g_PointLightDir[MAX_POINT_LIGHT_CNT];
	float4  g_PointLightRadius[MAX_POINT_LIGHT_CNT];
	float4x4 g_PointLightView[MAX_POINT_LIGHT_CNT];
	int  g_PointLightCnt;
	int g_PointLightShadowTextureSize;
	int g_PointDummy1;
	int g_PointDummy2;
}

cbuffer SpotLightData : register(b2)
{
	float4 g_SpotLightColor[MAX_SPOT_LIGHT_CNT];
	float4 g_SpotLightPos[MAX_SPOT_LIGHT_CNT];
	float4 g_SpotLightDir[MAX_SPOT_LIGHT_CNT];
	float4 g_SpotLightRadius[MAX_SPOT_LIGHT_CNT];
	float4 g_SpotLightDistance[MAX_SPOT_LIGHT_CNT];
	float4x4 g_SpotLightView[MAX_SPOT_LIGHT_CNT];
	int  g_SpotLightCnt;
	int g_SpotLightShadowTextureSize;
	int g_SpotDummy1;
	int g_SpotDummy2;
}

cbuffer CameraData : register(b3)
{
	float4 g_EyePos;
	float4 g_EyeDir;
}

cbuffer TransformData : register(b4)
{
	matrix g_WorldTransform : packoffset(c0);
	matrix g_InversedWorldTransform : packoffset(c4);
}

cbuffer MaterialData : register(b5)
{
	float4 g_DiffuseMaterial;
	float4 g_AmbientMaterial;
	float4 g_SpecularMaterial;
	float4 g_EmissionMaterial;
}

cbuffer CBUF_MOTION_TRAIL_DATA : register(b6)
{
	float3			pos					: packoffset(c0.x);
	float			timer				: packoffset(c0.w);

	float3			direction			: packoffset(c1.x);
	float			hitColorDuration	: packoffset(c1.w);
};

Texture2D		g_DiffuseTexture	: register(t0); // register를 안 붙여주면 기본적으로 0번 레지스터에 들어감.
Texture2DArray	g_DirectionalLightShadowDepthMap	: register(t6);
Texture2DArray	g_PointLightShadowDepthMap	: register(t7);
Texture2DArray	g_SpotLightShadowDepthMap	: register(t8);
SamplerState	g_SampleA		: register(s0); // 샘플링을 하려면 샘플러가 필요함
SamplerComparisonState	g_SampleCompShadowMap : register(s1);

float4 calculateRimRight(float4 cRimColor, float3 vCamDir, float3 vNormal, float fAdjustIntesity)
{
	vCamDir = normalize(vCamDir);
	vNormal = normalize(vNormal);

	float RimPower = 1.0f - saturate(dot(-vCamDir, vNormal) - fAdjustIntesity);

	return cRimColor * RimPower;
}

// SV_Target: 출력 영역에 뿌릴 색상.
float4 PS(PixelShader_input _input) : SV_Target
{
	float4 finalColor = float4(0.0f, 0.5f, 1.0f, 0.0f);
	float4 rimLightColor = float4(0.75f, 0.75f, 0.75f, 1.0f);

	float fAdjustIntesity = 0.1f;
	
	//림 라이트
	float4 RimColor = calculateRimRight(rimLightColor,
	_input.vWorld.xyz - g_EyePos.xyz,
	_input.n,
	fAdjustIntesity);
	
	finalColor += RimColor * (1.0f - smoothstep(0.0f, hitColorDuration, timer));

	return finalColor;
}