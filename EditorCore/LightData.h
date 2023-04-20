#pragma once
#include "Define.h"
#define MAX_DIRECTIONAL_LIGHT_CNT (int)10
#define MAX_POINT_LIGHT_CNT (int)10
#define MAX_SPOT_LIGHT_CNT (int)10

struct DirectionalLightData
{
	Vector4 Color[MAX_DIRECTIONAL_LIGHT_CNT];
	Vector4 Position[MAX_DIRECTIONAL_LIGHT_CNT];
	Vector4 Direction[MAX_DIRECTIONAL_LIGHT_CNT];
	Matrix	View[MAX_DIRECTIONAL_LIGHT_CNT];
	int Cnt;
	int ShadowTextureSize;
	int dummy1;
	int dummy2;
};

struct PointLightData
{
	Vector4 Color[MAX_POINT_LIGHT_CNT];
	Vector4 Position[MAX_POINT_LIGHT_CNT];
	Vector4 Direction[MAX_POINT_LIGHT_CNT];
	Vector4 Radius[MAX_POINT_LIGHT_CNT];
	Matrix	View[MAX_POINT_LIGHT_CNT];
	int Cnt = 0;
	int ShadowTextureSize;
	int dummy1;
	int dummy2;
};

struct SpotLightData
{
	Vector4 Color[MAX_SPOT_LIGHT_CNT];
	Vector4 Position[MAX_SPOT_LIGHT_CNT];
	Vector4 Direction[MAX_SPOT_LIGHT_CNT];
	Vector4 Radius[MAX_SPOT_LIGHT_CNT];
	Vector4 Distance[MAX_SPOT_LIGHT_CNT];
	Matrix	View[MAX_SPOT_LIGHT_CNT];
	int Cnt = 0;
	int ShadowTextureSize;
	int dummy1;
	int dummy2;
};

struct EyeData
{
	Vector4 Position;
	Vector4 Direction;
};