struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR0; // COLOR0 �� COLOR1 �ۿ� ����.
	float2 t : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD7 (15) ���� ����.
	float4 lightColor : TEXCOORD1;
	float4 vWorld : TEXCOORD2;
	float3 light : TEXCOORD3;
};

cbuffer SculptingData : register(b0)
{
	float4 g_SculptingPosition;
	float g_BrushRadius;
	float g_Attenuation;
	float g_Strength;
	float g_HeightDir;
}

[maxvertexcount(3)]
void GS(triangle GSOutput input[3] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		float dist = distance(input[i].p.xyz, g_SculptingPosition.xyz);
		float result = 1.0f - smoothstep(g_BrushRadius * g_Attenuation, g_BrushRadius, dist);
	
		if (result > 0)
		{
			float yVal = g_Strength * g_HeightDir * result;
			element = input[i];
			element.p.y += yVal;
		}

		output.Append(element);
	}
}