#pragma once
#include "Define.h"


struct Camera
{
public:
	DirectX::BoundingFrustum Frustum;

public:
	Vector3 Pos;
	Vector3 Look;
	Vector3 Right;
	Vector3 Up;

	Matrix View;
	Matrix Projection;

public:
	float FarDistance;
	float NearDistance;
	float AspectRatio;
	float FovX;
	float FovY;
};