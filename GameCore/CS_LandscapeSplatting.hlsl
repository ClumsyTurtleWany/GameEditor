struct SplattingData
{
	float3 PickPosition;
	float  Radius;
	int	   Index;
	float  TextureWidth;
	float  TextureHeight;
};

Texture2D<float4> InputMap : register(t0);
Texture2D<float4> CopyMap  : register(t1);
StructuredBuffer<SplattingData> SplattingInfo : register(t2);
RWTexture2D<float4>	OutputMap : register(u0);

#define size_x 32
#define size_y 32

[numthreads(size_x, size_y, 1)]
void CS(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex)
{
	int3 texturelocation = int3(0, 0, 0);
	// 0 ~ 1024,
	texturelocation.x = GroupID.x * size_x + GroupThreadID.x; // u
	texturelocation.y = GroupID.y * size_y + GroupThreadID.y; // v
	//texturelocation.x = DispatchThreadID.x;
	//texturelocation.y = DispatchThreadID.y;
	float4 Color = InputMap.Load(texturelocation);
	// 0 ~1 
	float2 uv = float2(texturelocation.x / SplattingInfo[0].TextureWidth,
		texturelocation.y / SplattingInfo[0].TextureHeight);
	// vRect[0]   ~   vRect[1]  
	float1 width = SplattingInfo[0].TextureWidth / 2.0f;
	// vRect[3]   ~   vRect[2]  
	float1 height = SplattingInfo[0].TextureWidth / 2.0f;
	float3 vPos = float3((uv.x * 2.0f - 1.0f) * width, -(uv.y * 2.0f - 1.0f) * height, 0.0f);

	float fRadius = distance(vPos.xyz, SplattingInfo[0].PickPosition.xyz);
	float4 fAlpha = CopyMap.Load(texturelocation);
	if (fRadius < SplattingInfo[0].Radius)
	{
		float fDot = 1.0f - (fRadius / SplattingInfo[0].Radius);
		switch (SplattingInfo[0].Index)
		{
		case 0: fAlpha.x = max(fAlpha.x, fDot); break;
		case 1: fAlpha.y = max(fAlpha.y, fDot); break;
		case 2: fAlpha.z = max(fAlpha.z, fDot); break;
		case 3: fAlpha.w = max(fAlpha.w, fDot); break;
		}
		OutputMap[texturelocation.xy] = fAlpha;
	}
}