#pragma once
#include "Define.h"
#include "LightData.h"

struct DirectionalLightComponent
{
	Vector4 Color;
	Vector4 Direction;
};

struct PointLightComponent
{
	Vector4 Color;
	Vector4 Direction;
	float Radius;
};

struct SpotLightComponent
{
	Vector4 Color;
	Vector4 Direction;
	float Radius;
};