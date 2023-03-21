#pragma once
#include "Define.h"

class BoundingBoxComponent
{
public:
	DirectX::BoundingOrientedBox InitOBB;
	DirectX::BoundingOrientedBox OBB;

public:
	BoundingBoxComponent() {}

	BoundingBoxComponent(Vector3 exts, Vector3 center = {0.0f, 0.0f, 0.0f})
	{
		InitOBB.Center = center;
		InitOBB.Extents = exts;
		InitOBB.Orientation = Quaternion(Vector3(0.0f, 0.0f, 0.0f), 1.0f);
	}

	BoundingBoxComponent(Vector3 center, Vector3 exts, Quaternion rotation)
	{
		InitOBB.Center = center;
		InitOBB.Extents = exts;
		InitOBB.Orientation = rotation;
	}

	~BoundingBoxComponent() {}
};
