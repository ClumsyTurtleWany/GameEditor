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

Texture2D		g_DiffuseTexture	: register(t0); // register를 안 붙여주면 기본적으로 0번 레지스터에 들어감.
Texture2D		g_NormalMapTexture	: register(t1);
Texture2DArray	g_DirectionalLightShadowDepthMap	: register(t6);
Texture2DArray	g_PointLightShadowDepthMap	: register(t7);
Texture2DArray	g_SpotLightShadowDepthMap	: register(t8);
SamplerState	g_SampleA		: register(s0); // 샘플링을 하려면 샘플러가 필요함
SamplerComparisonState	g_SampleCompShadowMap : register(s1);

float4 ComputeDirectionDiffuseLight(float3 normal, float3x3 tangent)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int idx = 0; idx < g_DirectionLightCnt; idx++)
	{
		float intensity = saturate(max(0, dot(normal, -normalize(mul(g_DirectionLightDir[idx].xyz, tangent)))));
		outputColor += g_DirectionLightColor[idx] * intensity;
		//outputColor += ambientColor;
		//outputColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	outputColor.w = 1.0f;
	return outputColor;
}

float4 ComputePointDiffuseLight(float3 pos, float3 normal, float3x3 tangent)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_PointLightCnt; idx++)
	{
		// Diffuse
		float3 vLight = normalize(pos - g_PointLightPos[idx].xyz);
		vLight = normalize(mul(vLight, tangent));
		float dist = distance(pos, g_PointLightPos[idx].xyz);

		float luminance = smoothstep(dist - 5.0f, dist, g_PointLightRadius[idx].x);
		float intensity = saturate(dot(normal, -vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor; // +ambientColor;
}

float4 ComputeSpotDiffuseLight(float3 pos, float3 normal, float3x3 tangent)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_SpotLightCnt; idx++)
	{
		// Diffuse
		float3 vLight = normalize(pos - g_SpotLightPos[idx].xyz);
		vLight = normalize(mul(vLight, tangent));
		float dist = distance(pos, g_SpotLightPos[idx].xyz);

		float fDot = dot(g_SpotLightDir[idx].xyz, vLight);
		float luminance = smoothstep(dist - 5.0f, dist, g_SpotLightRadius[idx].x);
		float intensity = saturate(dot(normal, -vLight));

		if (fDot >= 0.98)
		{
			intensity = saturate(dot(normal, -vLight));
			outputColor += float4(g_SpotLightColor[idx].rgb * luminance * intensity, 1.0f);
		}
		else
		{
			//float lerpRadius = g_SpotLightRadius[idx] / 2.0f;
			if (fDot >= 0.9)
			{
				float luminance2 = smoothstep(0.9, 0.98, fDot);
				outputColor += float4(g_SpotLightColor[idx].rgb * min(min(luminance2, luminance), intensity), 1.0f);
			}
		}
	}

	//outputColor.x = g_SpotLightRadius[0];
	//outputColor.z = g_SpotLightRadius[0];

	return outputColor; // +ambientColor;
}

float4 ComputePointSpecularLight(float3 pos, float3 normal, float3x3 tangent)
{
	// 빛이 반사되어 눈에 들어오는 것이 Specular, 조명의 결과가 시선 벡터에 따라 달라진다. 반사벡터와 시선벡터의 내적.
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_PointLightCnt; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_PointLightDir[idx].xyz, normal.xyz);
		vLight = normalize(mul(vLight, tangent));
		float dist = distance(pos, g_PointLightPos[idx].xyz);

		float luminance = smoothstep(dist - 5.0f, dist, g_PointLightRadius[idx].x);
		float intensity = saturate(dot(-normalize(mul(g_EyeDir, tangent)), vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor;// +ambientColor;
}

float4 ComputeSpotSpecularLight(float3 pos, float3 normal, float3x3 tangent)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_SpotLightCnt; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_SpotLightDir[idx].xyz, normal.xyz);
		vLight = normalize(mul(vLight, tangent));
		float dist = distance(pos, g_SpotLightPos[idx].xyz);

		float fDot = dot(g_SpotLightDir[idx], vLight);
		float luminance = smoothstep(dist - 5.0f, dist, g_SpotLightRadius[idx].x);
		float intensity = saturate(dot(normalize(mul(g_EyeDir, tangent)), vLight));

		if (fDot >= 0.98)
		{
			intensity = saturate(dot(normal, -vLight));
			outputColor += float4(g_SpotLightColor[idx].rgb * luminance * intensity, 1.0f);
		}
		else
		{
			//float lerpRadius = g_PointLightRadius[idx] / 2.0f;
			if (fDot >= 0.9)
			{
				float luminance2 = smoothstep(0.9, 0.98, fDot);
				outputColor += float4(g_SpotLightColor[idx].rgb * min(min(luminance2, luminance), intensity), 1.0f);
			}
		}
	}

	return outputColor;// +ambientColor;
}

//---------------------------------------------------------------------------------------
// Compute Shadow
//---------------------------------------------------------------------------------------
float ComputeDirectionalLightShadowAmount(float4 pos)
{
	// Shadow Texture Coordinate
	const float delta = 1.0f / g_DirectionLightShadowTextureSize;
	float finalLightAmount = 0.0f;
	int lightCnt = g_DirectionLightCnt;
	for (int idx = 0; idx < lightCnt; idx++)
	{
		float4 shadowUV = mul(pos, g_DirectionalLightView[idx]);
		float3 shadowTexColor = shadowUV.xyz / shadowUV.w;
		float lightAmount = 0.0f;
		for (int v = -1; v <= 1; v++)
		{
			for (int u = -1; u <= 1; u++)
			{
				float2 offset = float2(u * delta, v * delta);
				lightAmount += g_DirectionalLightShadowDepthMap.SampleCmpLevelZero(g_SampleCompShadowMap, float3(shadowTexColor.xy + offset, idx), shadowTexColor.z).r;
			}
		}
		lightAmount /= (3 * 3);
		finalLightAmount += lightAmount;
	}
	finalLightAmount = finalLightAmount / (lightCnt + EPSILON);
	return finalLightAmount;
}

float ComputeSpotLightShadowAmount(float4 pos)
{
	// Shadow Texture Coordinate
	const float delta = 1.0f / g_SpotLightShadowTextureSize;
	float finalLightAmount = 0.0f;
	int lightCnt = g_SpotLightCnt;
	for (int idx = 0; idx < lightCnt; idx++)
	{
		float4 shadowUV = mul(pos, g_SpotLightView[idx]);
		float3 shadowTexColor = shadowUV.xyz / shadowUV.w;
		float lightAmount = 0.0f;
		for (int v = -1; v <= 1; v++)
		{
			for (int u = -1; u <= 1; u++)
			{
				float2 offset = float2(u * delta, v * delta);
				lightAmount += g_SpotLightShadowDepthMap.SampleCmpLevelZero(g_SampleCompShadowMap, float3(shadowTexColor.xy + offset, idx), shadowTexColor.z).r;
			}
		}
		lightAmount /= (3 * 3);
		finalLightAmount += lightAmount;
	}
	finalLightAmount = finalLightAmount / (lightCnt + EPSILON);
	return finalLightAmount;
}

float ComputePointLightShadowAmount(float4 pos)
{
	// Shadow Texture Coordinate
	const float delta = 1.0f / g_PointLightShadowTextureSize;
	float finalLightAmount = 0.0f;
	int lightCnt = g_PointLightCnt;
	for (int idx = 0; idx < lightCnt; idx++)
	{
		float4 shadowUV = mul(pos, g_PointLightView[idx]);
		float3 shadowTexColor = shadowUV.xyz / shadowUV.w;
		float lightAmount = 0.0f;
		for (int v = -1; v <= 1; v++)
		{
			for (int u = -1; u <= 1; u++)
			{
				float2 offset = float2(u * delta, v * delta);
				lightAmount += g_PointLightShadowDepthMap.SampleCmpLevelZero(g_SampleCompShadowMap, float3(shadowTexColor.xy + offset, idx), shadowTexColor.z).r;
			}
		}
		lightAmount /= (3 * 3);
		finalLightAmount += lightAmount;
	}
	finalLightAmount = finalLightAmount / (lightCnt + EPSILON);
	return finalLightAmount;
}




// SV_Target: 출력 영역에 뿌릴 색상.
float4 PS(PixelShader_input _input) : SV_Target
{
	float3 vNormal = normalize(mul(_input.n, (float3x3)g_InversedWorldTransform));
	float3 T = normalize(mul(_input.tangent, (float3x3)g_InversedWorldTransform));
	float3 B = normalize(cross(vNormal, T));
	float3x3 tanMat = { T.x, B.x, vNormal.x,
						T.y, B.y, vNormal.y,
						T.z, B.z, vNormal.z };


	float4 textureColor = g_DiffuseTexture.Sample(g_SampleA, _input.t);
	float4 normalMap = g_NormalMapTexture.Sample(g_SampleA, _input.t);
	normalMap = normalize((normalMap - 0.5f) * 2.0f);


	float4 lightColor = ComputeDirectionDiffuseLight(normalMap, tanMat) +
						ComputePointDiffuseLight(_input.vWorld, normalMap, tanMat) +
						ComputePointSpecularLight(_input.vWorld, normalMap, tanMat) +
						ComputeSpotDiffuseLight(_input.vWorld, normalMap, tanMat) +
						ComputeSpotSpecularLight(_input.vWorld, normalMap, tanMat);

	float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 finalColor = textureColor * (lightColor + ambientColor);
	
	float directionalLightShadowBright = ComputeDirectionalLightShadowAmount(_input.vWorld);
	float pointLightShadowBright = ComputePointLightShadowAmount(_input.vWorld);
	float spotLightShadowBright = ComputeSpotLightShadowAmount(_input.vWorld);
	float shadowBright = directionalLightShadowBright + pointLightShadowBright + spotLightShadowBright;
	shadowBright /= 3.0f;

	finalColor = finalColor * max(0.3f, shadowBright);
	finalColor.a = 1.0f;

	return finalColor;
}