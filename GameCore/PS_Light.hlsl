// 픽셀 마다 호출된다.
// 해당 픽셀의 컬러를 SV_Target 레지스터에 반환 한다.
// Rasterizer(== Pixel) Stage 에서 PixelShader 호출 할 횟수를 결정.

struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 과 COLOR1 밖에 없음.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) 까지 있음.
	float4 lightColor : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
	float3 light : TEXCOORD3;
};

cbuffer DirectionLightData : register(b0)
{
	float4 g_DirectionLightColor[255];
	float3 g_DirectionLightDir[255];
	int  g_DirectionLightCnt;
}

cbuffer SpotLightData : register(b1)
{
	float4 g_SpotLightColor[255];
	float3 g_SpotLightPos[255];
	float3 g_SpotLightDir[255];
	float3 g_SpotLightPoint[255];
	float3 g_SpotLightRadius[255];
	int  g_SpotLightCnt;
}

cbuffer PointLightData : register(b2)
{
	float4 g_PointLightColor[255];
	float3 g_PointLightPos[255];
	float3 g_PointLightDir[255];
	float3 g_PointLightPoint[255];
	float  g_PointLightRadius[255];
	int  g_PointLightCnt;
}

cbuffer CameraData : register(b3)
{
	float3 g_EyePos;
	float3 g_EyeDir;
}

Texture2D		g_DiffuseTexture	: register(t0); // register를 안 붙여주면 기본적으로 0번 레지스터에 들어감.
SamplerState	g_SampleA		: register(s0); // 샘플링을 하려면 샘플러가 필요함

float4 ComputePointDiffuseLight(float3 pos, float3 normal);
{
	float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_PointLightCnt; idx++)
	{
		// Diffuse
		float3 vLight = normalize(pos - g_PointLightPos[idx]);
		float distance = distance(pos, g_PointLightPos[idx]);

		float luminance = smoothstep(distance - 5.0f, distance, g_PointLightRadius[idx]);
		float intensity = saturate(dot(normal, vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor + ambientColor;
}

float4 ComputeSpotDiffuseLight(float3 pos, float3 normal);
{
	float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_SpotLightCnt; idx++)
	{
		// Diffuse
		float4 vLight = normalize(pos - g_SpotLightPos[idx]);
		float distance = distance(pos, g_SpotLightPos[idx]);

		float fDot = dot(g_SpotLightDir[idx].xyz, vLight);
		float luminance = smoothstep(distance - 5.0f, distance, g_PointLightRadius[idx]);
		float intensity = saturate(dot(normal, -vLight));

		if (fDot >= g_PointLightRadius[idx])
		{
			intensity = saturate(dot(normal, -vLight));
			outputColor += float4(g_SpotLightColor[idx].rgb * luminance * intensity, 1.0f);
		}
		else
		{
			float lerpRadius = g_PointLightRadius[idx] / 2.0f;
			if (fDot >= lerpRadius)
			{
				float luminance2 = smoothstep(lerpRadius, g_PointLightRadius[idx], fDot);
				outputColor += float4(g_SpotLightColor[idx].rgb * min(min(luminance2, luminance), intensity), 1.0f);
			}
		}
	}

	return outputColor + ambientColor;
}

float4 ComputePointSpecularLight(float3 pos, float3 normal);
{
	// 빛이 반사되어 눈에 들어오는 것이 Specular, 조명의 결과가 시선 벡터에 따라 달라진다. 반사벡터와 시선벡터의 내적.
	float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_PointLightCnt; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_PointLightDir[idx], normal);
		float distance = distance(pos, g_PointLightPos[idx]);

		float luminance = smoothstep(distance - 5.0f, distance, g_PointLightRadius[idx]);
		float intensity = saturate(dot(-g_EyeDir, vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor + ambientColor;
}

float4 ComputeSpotSpecularLight(float3 pos, float3 normal);
{
	float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < g_SpotLightCnt; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_SpotLightDir[idx] - normal);
		float distance = distance(pos, g_SpotLightPos[idx]);

		float fDot = dot(g_SpotLightDir[idx], vLight);
		float luminance = smoothstep(distance - 5.0f, distance, g_PointLightRadius[idx]);
		float intensity = saturate(dot(g_EyeDir, vLight));

		if (fDot >= g_PointLightRadius[idx])
		{
			intensity = saturate(dot(normal, -vLight));
			outputColor += float4(g_SpotLightColor[idx].rgb * luminance * intensity, 1.0f);
		}
		else
		{
			float lerpRadius = g_PointLightRadius[idx] / 2.0f;
			if (fDot >= lerpRadius)
			{
				float luminance2 = smoothstep(lerpRadius, g_PointLightRadius[idx], fDot);
				outputColor += float4(g_SpotLightColor[idx].rgb * min(min(luminance2, luminance), intensity), 1.0f);
			}
		}
	}

	return outputColor + ambientColor;
}

// SV_Target: 출력 영역에 뿌릴 색상.
float4 PS(PixelShader_input _input) : SV_Target
{
	float4 textureColor = g_DiffuseTexture.Sample(g_SampleA, _input.t);
	float4 lightColor = ComputePointDiffuseLight(input.w, input.n, 1) +
						ComputePointSpecularLight(input.w, input.n, 1) +
						ComputeSpotDiffuseLight(input.w, input.n, 1) +
						ComputeSpotSpecularLight(input.w, input.n, 1);

	float4 finalColor = textureColor * lightColor;
	finalColor.a = 1.0f;
	
	return finalColor;
}