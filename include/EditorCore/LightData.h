#pragma once
#include "Define.h"
#define MAX_LIGHT_CNT (int)20

struct DirectionalLightData
{
	Vector4 Color[MAX_LIGHT_CNT];
	Vector4 Direction[MAX_LIGHT_CNT];
	int Cnt;
	int dummy1;
	int dummy2;
	int dummy3;
};

struct SpotLightData
{
	Vector4 Color[MAX_LIGHT_CNT];
	Vector4 Position[MAX_LIGHT_CNT];
	Vector4 Direction[MAX_LIGHT_CNT];
	Vector4 Radius[MAX_LIGHT_CNT];
	int Cnt = 0;
	int dummy1;
	int dummy2;
	int dummy3;
};

struct PointLightData
{
	Vector4 Color[MAX_LIGHT_CNT];
	Vector4 Position[MAX_LIGHT_CNT];
	Vector4 Direction[MAX_LIGHT_CNT];
	Vector4 Radius[MAX_LIGHT_CNT];
	int Cnt = 0;
	int dummy1;
	int dummy2;
	int dummy3;
};

struct EyeData
{
	Vector4 Position;
	Vector4 Direction;
};