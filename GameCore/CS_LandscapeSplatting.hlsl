struct SplattingData
{
	float3 PickPosition;
	int	   Index;
	int		DispatchX;
	int		DispatchY;
	float  Radius;
	float  Strength;
	float3 MinVertex;
	float3 MaxVertex;
	//float  TextureWidth;
	//float  TextureHeight;
};

Texture2D<float4> InputMap : register(t0);
Texture2D<float4> CopyMap  : register(t1);
StructuredBuffer<SplattingData> SplattingInfo : register(t2);
RWTexture2D<float4>	OutputMap : register(u0);

#define size_x 32
#define size_y 32

// Dispatch(x, y, z)
// x: column
// y: row
// z: depth
// example) 
// Dispatch(5, 3, 1) == SV_GroupID[1][3][5] == 15개의 그룹
// numthreads(10, 8, 3) == SV_GroupThreadID[3][8][10]
// 
// 예시)
// Dispatch(32, 32, 1), numthreads(32, 32, 1) 일 때,
// SV_GroupID.x: 0 ~ 31
// SV_GroupID.y: 0 ~ 31
// SV_GroupThreadID.x: 0 ~ 31
// SV_GroupThreadID.y: 0 ~ 31
// SV_GroupID (2, 1, 0) 에 SV_GroupThreadID (7, 5, 0) 의 경우 
// SV_DispatchThreadID = (2, 1, 0) * (32, 32, 1) + (7, 5, 0) = (71, 37, 0)
// SV_GroupIndex = 0 * 32 * 32 + 5 * 32 + 7 = 167
// x * y * z는 최대 1024를 넘으면 안됨. 보통 2차원 배열 기준 32 * 32 * 1 사용
// 32 * 32 * 1로 하면
// SV_GroupID[32][32] 로 보면 됨.
// SV_GroupIndex: 0 ~ ((size_x * size_y * 1) - 1)

//[numthreads(size_x, size_y, 1)]
//void CS(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex)
//{
//	int3 texturelocation = int3(0, 0, 0);
//
//	// 0 ~ 1023, Dispatch(32, 32, 1)에 numthreads(32, 32, 1) 이므로
//	// 텍스처를 x: 1024개, y: 1024개 로 쪼갬.
//	texturelocation.x = GroupID.x * size_x + GroupThreadID.x; // u
//	texturelocation.y = GroupID.y * size_y + GroupThreadID.y; // v
//
//	float4 Color = InputMap.Load(texturelocation);
//	float textureWidth = size_x * SplattingInfo[0].TextureWidth;
//	float textureHeight = size_y * SplattingInfo[0].TextureHeight;
//	// 0 ~1 
//	float2 uv = float2(texturelocation.x / textureWidth, texturelocation.y / textureHeight);
//	// vRect[0]   ~   vRect[1]  
//	float1 width = SplattingInfo[0].TextureWidth / 2.0f; // 256 / 2 = 128
//	// vRect[3]   ~   vRect[2]  
//	float1 height = SplattingInfo[0].TextureHeight / 2.0f; // 256 / 2 = 128
//	float3 vPos = float3((uv.x * 2.0f - 1.0f) * width, -(uv.y * 2.0f - 1.0f) * height, 0.0f);
//
//	float fRadius = distance(vPos.xy, SplattingInfo[0].PickPosition.xy);
//	float4 fAlpha = CopyMap.Load(texturelocation);
//	if (fRadius < SplattingInfo[0].Radius)
//	{
//		float fDot = (1.0f - (fRadius / SplattingInfo[0].Radius)) * SplattingInfo[0].Strength;
//		switch (SplattingInfo[0].Index)
//		{
//		case 0: fAlpha.x = max(fAlpha.x, fDot); break;
//		case 1: fAlpha.y = max(fAlpha.y, fDot); break;
//		case 2: fAlpha.z = max(fAlpha.z, fDot); break;
//		case 3: fAlpha.w = max(fAlpha.w, fDot); break;
//		}
//		OutputMap[texturelocation.xy] = fAlpha;
//	}
//
//	OutputMap[DispatchThreadID.xy / 2] = float4(1.0f, 0.0f, 0.0f, 0.0f);
//}

[numthreads(size_x, size_y, 1)]
void CS(uint3 GroupID : SV_GroupID, uint3 DispatchThreadID : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint GroupIndex : SV_GroupIndex)
{
	int3 texturelocation = int3(0, 0, 0);

	// 0 ~ 1023, Dispatch(32, 32, 1)에 numthreads(32, 32, 1) 이므로
	// 텍스처를 x: 1024개, y: 1024개 로 쪼갬.
	texturelocation.x = GroupID.x * size_x + GroupThreadID.x; // u
	texturelocation.y = GroupID.y * size_y + GroupThreadID.y; // v

	float4 Color = InputMap.Load(texturelocation);
	float textureWidth = size_x * SplattingInfo[0].DispatchX;
	float textureHeight = size_y * SplattingInfo[0].DispatchY;
	// 0 ~1 
	float2 uv = float2(texturelocation.x / textureWidth, texturelocation.y / textureHeight);
	
	float3 diff = SplattingInfo[0].MaxVertex - SplattingInfo[0].MinVertex; // (7, 0, 7) - (0, 0, 0) = (7, 0, 7)
	// (14, 0, 7) - (7, 0, 0) = (7, 0, 7)

	float3 vPos = SplattingInfo[0].MinVertex + float3(uv.x * diff.x, 0.0f, uv.y * diff.z);

	float dist = distance(vPos.xz, SplattingInfo[0].PickPosition.xz);
	float4 fAlpha = CopyMap.Load(texturelocation);
	if (dist < SplattingInfo[0].Radius)
	{
		float fDot = (1.0f - (dist / SplattingInfo[0].Radius)) * SplattingInfo[0].Strength;
		float sub = fDot / 3.0f;
		switch (SplattingInfo[0].Index)
		{
		case 0:
		{
			fAlpha.x = min(fAlpha.x + fDot, 1.0f);
			fAlpha.y = max(0.0f, fAlpha.y - sub);
			fAlpha.z = max(0.0f, fAlpha.z - sub);
			fAlpha.w = max(0.0f, fAlpha.w - sub);
			break;
		}
		case 1: 
		{
			fAlpha.y = min(fAlpha.y + fDot, 1.0f);
			fAlpha.x = max(0.0f, fAlpha.x - sub);
			fAlpha.z = max(0.0f, fAlpha.z - sub);
			fAlpha.w = max(0.0f, fAlpha.w - sub);
			break;
		}
		case 2: 
		{
			fAlpha.z = min(fAlpha.z + fDot, 1.0f);
			fAlpha.x = max(0.0f, fAlpha.x - sub);
			fAlpha.y = max(0.0f, fAlpha.y - sub);
			fAlpha.w = max(0.0f, fAlpha.w - sub);
			break;
		}
		case 3: 
		{
			fAlpha.w = min(fAlpha.w + fDot, 1.0f);
			fAlpha.x = max(0.0f, fAlpha.x - sub);
			fAlpha.y = max(0.0f, fAlpha.y - sub);
			fAlpha.z = max(0.0f, fAlpha.z - sub);
			break;
		}
		}
		OutputMap[texturelocation.xy] = fAlpha;
	}

	//OutputMap[DispatchThreadID.xy / 2] = float4(1.0f, 0.0f, 0.0f, 0.0f);
}