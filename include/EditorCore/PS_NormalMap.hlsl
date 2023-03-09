// �ȼ� ���� ȣ��ȴ�.
// �ش� �ȼ��� �÷��� SV_Target �������Ϳ� ��ȯ �Ѵ�.
// Rasterizer(== Pixel) Stage ���� PixelShader ȣ�� �� Ƚ���� ����.

struct PixelShader_input
{
	float4 p : SV_POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
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

cbuffer TransformData : register(b4)
{
	matrix g_WorldTransform : packoffset(c0);
	matrix g_InversedWorldTransform : packoffset(c4);
}

Texture2D		g_DiffuseTexture	: register(t0); // register�� �� �ٿ��ָ� �⺻������ 0�� �������Ϳ� ��.
Texture2D		g_NormalMapTexture	: register(t1);
SamplerState	g_SampleA		: register(s0); // ���ø��� �Ϸ��� ���÷��� �ʿ���

float4 ComputeDirectionDiffuseLight(float3 normal, float3x3 tangent)
{
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int idx = 0; idx < 20; idx++)
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
	for (int idx = 0; idx < 20; idx++)
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
	for (int idx = 0; idx < 20; idx++)
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
	// ���� �ݻ�Ǿ� ���� ������ ���� Specular, ������ ����� �ü� ���Ϳ� ���� �޶�����. �ݻ纤�Ϳ� �ü������� ����.
	//float4 ambientColor = float4(0.3f, 0.3f, 0.3f, 1.0f);
	float4 outputColor = float4(0, 0, 0, 1);
	for (int idx = 0; idx < 20; idx++)
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
	for (int idx = 0; idx < 20; idx++)
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

// SV_Target: ��� ������ �Ѹ� ����.
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
	//finalColor = float4(_input.tangent.x, _input.tangent.y, _input.tangent.z, 1.0f);
	finalColor.a = 1.0f;

	return finalColor;
}