#pragma once
#include "Define.h"

struct SplattingData
{
	Vector3 Position;
	int Index;
	int DispatchX;
	int DispatchY;
	float Radius;
	float Strength;
	Vector3 MinVertex;
	Vector3 MaxVertex;
	//float TextureWidth;
	//float TextureHeight;
};