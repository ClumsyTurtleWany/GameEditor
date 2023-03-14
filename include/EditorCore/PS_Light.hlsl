// 픽셀 마다 호출된다.
// 해당 픽셀의 컬러를 SV_Target 레지스터에 반환 한다.
// Rasterizer(== Pixel) Stage 에서 PixelShader 호출 할 횟수를 결정.

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
	float4 g_DirectionLightColor[20];
	float3 g_DirectionLightDir[20];
	int  g_DirectionLightCnt;
	int g_DirectionalDummy1;
	int g_DirectionalDummy2;
	int g_DirectionalDummy3;
}

cbuffer PointLightData : register(b1)
{
	float4 g_PointLightColor[20];
	float4 g_PointLightPos[20];
	float4 g_PointLightDir[20];
	float4  g_PointLightRadius[20];
	int  g_PointLightCnt;
	int g_PointDummy1;
	int g_PointDummy2;
	int g_PointDummy3;
}

cbuffer SpotLightData : register(b2)
{
	float4 g_SpotLightColor[20];
	float4 g_SpotLightPos[20];
	float4 g_SpotLightDir[20];
	float4 g_SpotLightRadius[20];
	int  g_SpotLightCnt;
	int g_SpotDummy1;
	int g_SpotDummy2;
	int g_SpotDummy3;
}

cbuffer CameraData : register(b3)
{
	float4 g_EyePos;
	float4 g_EyeDir;
}

cbuffer MaterialData : register(b4)
{
	float4 g_DiffuseMaterial;
	float4 g_AmbientMaterial;
	float4 g_SpecularMaterial;
	float4 g_EmissionMaterial;
}

Texture2D		g_DiffuseTexture	: register(t0); // register를 안 붙여주면 기본적으로 0번 레지스터에 들어감.
SamplerState	g_SampleA		: register(s0); // 샘플링을 하려면 샘플러가 필요함

float4 ComputeDirectionDiffuseLight(float3 normal)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int idx = 0; idx < 20; idx++)
	{
		float intensity = max(0, dot(normal, normalize(-g_DirectionLightDir[idx].xyz)));
		outputColor += g_DirectionLightColor[idx] * intensity;
		//outputColor += ambientColor;
		//outputColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	outputColor.w = 1.0f;
	return outputColor;
}

float4 ComputePointDiffuseLight(float3 pos, float3 normal)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 20; idx++)
	{
		// Diffuse
		float3 vLight = normalize(pos - g_PointLightPos[idx].xyz);
		float dist = distance(pos, g_PointLightPos[idx].xyz);

		float luminance = smoothstep(dist - 5.0f, dist, g_PointLightRadius[idx].x);
		float intensity = saturate(dot(normal, -vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor; // +ambientColor;
}

float4 ComputeSpotDiffuseLight(float3 pos, float3 normal)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 20; idx++)
	{
		// Diffuse
		float3 vLight = normalize(pos - g_SpotLightPos[idx].xyz);
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

float4 ComputePointSpecularLight(float3 pos, float3 normal)
{
	// 빛이 반사되어 눈에 들어오는 것이 Specular, 조명의 결과가 시선 벡터에 따라 달라진다. 반사벡터와 시선벡터의 내적.
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 20; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_PointLightDir[idx].xyz, normal.xyz);
		float dist = distance(pos, g_PointLightPos[idx].xyz);

		float luminance = smoothstep(dist - 5.0f, dist, g_PointLightRadius[idx].x);
		float intensity = saturate(dot(-g_EyeDir, vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor;// +ambientColor;
}

float4 ComputeSpotSpecularLight(float3 pos, float3 normal)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 20; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_SpotLightDir[idx].xyz, normal.xyz);;
		float dist = distance(pos, g_SpotLightPos[idx].xyz);

		float fDot = dot(g_SpotLightDir[idx], vLight);
		float luminance = smoothstep(dist - 5.0f, dist, g_SpotLightRadius[idx].x);
		float intensity = saturate(dot(g_EyeDir, vLight));

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

// SV_Target: 출력 영역에 뿌릴 색상.
float4 PS(PixelShader_input _input) : SV_Target
{
	float4 textureColor = g_DiffuseTexture.Sample(g_SampleA, _input.t);
	float4 lightColor = ComputeDirectionDiffuseLight(_input.n) +
						ComputePointDiffuseLight(_input.vWorld, _input.n) +
						ComputePointSpecularLight(_input.vWorld, _input.n) +
						ComputeSpotDiffuseLight(_input.vWorld, _input.n) +
						ComputeSpotSpecularLight(_input.vWorld, _input.n);

	float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 finalColor = textureColor * (lightColor + ambientColor);
	finalColor.a = 1.0f;

	return finalColor;
}