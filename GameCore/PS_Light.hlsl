// �ȼ� ���� ȣ��ȴ�.
// �ش� �ȼ��� �÷��� SV_Target �������Ϳ� ��ȯ �Ѵ�.
// Rasterizer(== Pixel) Stage ���� PixelShader ȣ�� �� Ƚ���� ����.

struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
	float4 lightColor : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
	float3 light : TEXCOORD3;
};

cbuffer DirectionLightData : register(b0)
{
	float4 g_DirectionLightColor[100];
	float3 g_DirectionLightDir[100];
	int  g_DirectionLightCnt;
	int g_DirectionalDummy1;
	int g_DirectionalDummy2;
	int g_DirectionalDummy3;
}

cbuffer PointLightData : register(b1)
{
	float4 g_PointLightColor[100];
	float3 g_PointLightPos[100];
	float3 g_PointLightDir[100];
	float  g_PointLightRadius[100];
	int  g_PointLightCnt;
	int g_PointDummy1;
	int g_PointDummy2;
	int g_PointDummy3;
}

cbuffer SpotLightData : register(b2)
{
	float4 g_SpotLightColor[100];
	float3 g_SpotLightPos[100];
	float3 g_SpotLightDir[100];
	float g_SpotLightRadius[100];
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

Texture2D		g_DiffuseTexture	: register(t0); // register�� �� �ٿ��ָ� �⺻������ 0�� �������Ϳ� ��.
SamplerState	g_SampleA		: register(s0); // ���ø��� �Ϸ��� ���÷��� �ʿ���

float4 ComputeDirectionDiffuseLight(float3 normal)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int idx = 0; idx < 100; idx++)
	{
		float intensity = max(0, dot(normal, normalize(-g_DirectionLightDir[idx])));
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
	for (int idx = 0; idx < 100; idx++)
	{
		// Diffuse
		float3 vLight = normalize(pos - g_PointLightPos[idx]);
		float dist = distance(pos, g_PointLightPos[idx]);

		float luminance = smoothstep(dist - 5.0f, dist, g_PointLightRadius[idx]);
		float intensity = saturate(dot(normal, -vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor; // +ambientColor;
}

float4 ComputeSpotDiffuseLight(float3 pos, float3 normal)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 100; idx++)
	{
		// Diffuse
		float3 vLight = normalize(pos - g_SpotLightPos[idx]);
		float dist = distance(pos, g_SpotLightPos[idx]);

		float fDot = dot(g_SpotLightDir[idx].xyz, vLight);
		float luminance = smoothstep(dist - 5.0f, dist, g_SpotLightRadius[idx]);
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
	// ���� �ݻ�Ǿ� ���� ������ ���� Specular, ������ ����� �ü� ���Ϳ� ���� �޶�����. �ݻ纤�Ϳ� �ü������� ����.
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 100; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_PointLightDir[idx].xyz, normal.xyz);
		float dist = distance(pos, g_PointLightPos[idx]);

		float luminance = smoothstep(dist - 5.0f, dist, g_PointLightRadius[idx]);
		float intensity = saturate(dot(-g_EyeDir, vLight));

		outputColor += float4(g_PointLightColor[idx].rgb * luminance * intensity, 1.0f);
	}

	return outputColor;// +ambientColor;
}

float4 ComputeSpotSpecularLight(float3 pos, float3 normal)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 100; idx++)
	{
		// Diffuse
		float3 vLight = reflect(g_SpotLightDir[idx].xyz, normal.xyz);;
		float dist = distance(pos, g_SpotLightPos[idx]);

		float fDot = dot(g_SpotLightDir[idx], vLight);
		float luminance = smoothstep(dist - 5.0f, dist, g_SpotLightRadius[idx]);
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

// SV_Target: ��� ������ �Ѹ� ����.
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