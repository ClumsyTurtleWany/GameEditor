#pragma once
#include "Define.h"

class BoundingSphereComponent
{
public:
	DirectX::BoundingSphere InitSphere;
	DirectX::BoundingSphere Sphere;

public:
	BoundingSphereComponent() {}

	BoundingSphereComponent(float radius, Vector3 center = {0.0f, 0.0f, 0.0f})
	{
		InitSphere.Center = center;
		InitSphere.Radius = radius;
	}

	BoundingSphereComponent(DirectX::BoundingOrientedBox& OBB)
	{
		DirectX::BoundingSphere::CreateFromBoundingBox(Sphere, OBB);
	}

	~BoundingSphereComponent() {}
};

