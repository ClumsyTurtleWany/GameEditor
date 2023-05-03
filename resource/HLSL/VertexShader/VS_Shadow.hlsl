struct VSInputShadow
{
	float3 p : POSITION;
	float3 n : NORMAL;
	float4 c : COLOR;
	float2 t : TEXTURE;
	float3 tangent : TANGENT;
};

struct VSOutputShadow
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD0;
	// For Debug
	float4 world : TEXCOORD1;
	float4 view : TEXCOORD2;
	float4 proj : TEXCOORD3;
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

cbuffer MaterialData : register(b3)
{
	float4 g_DiffuseColor;
	float4 g_AmbientColor;
	float4 g_SpecularColor;
	float4 g_EmissiveColor;
	float4 g_ColorFilter;

	float4 g_Reflection;
	float4 g_Refraction;
	float4 g_Alpha;
	float4 g_Intensive;
}

VSOutputShadow VS(VSInputShadow input)
{
	VSOutputShadow output = (VSOutputShadow)0;
	float4 world = mul(float4(input.p, 1.0f), g_WorldTransform);
	float4 view = mul(world, g_View);
	float4 proj = mul(view, g_Projection);
	output.pos = proj;

	float alpha = min(g_Alpha, input.c.w);
	if (alpha <= 0.98)
	{
		output.pos.z = 1.0f * (1.0f - alpha);
		output.pos.w = 1.0f;
	}

	output.depth = output.pos.zw;
	output.world = world;
	output.view = view;
	output.proj = proj;

	return output;
}