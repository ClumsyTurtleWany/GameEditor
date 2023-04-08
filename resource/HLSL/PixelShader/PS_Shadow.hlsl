struct VSOutputShadow
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD0;
	// For Debug
	float4 world : TEXCOORD1;
	float4 view : TEXCOORD2;
	float4 proj : TEXCOORD3;
};

float4 PS(VSOutputShadow input) : SV_TARGET
{
	float shadow = input.depth.x / input.depth.y;
	return float4(shadow, shadow, shadow, 1.0f);
}