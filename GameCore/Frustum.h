#pragma once
#include "Define.h"
#include <DirectXCollision.h>

class Frustum
{
public:
	Vector3 FrustumVertices[8];
	Plane FrustumPlane[6];
	DirectX::BoundingFrustum Frustum;

public:
	Matrix View;
	Matrix Projection;

public:
	bool CreateFrustum(Matrix view, Matrix proj);
	bool ClassifyPoint(Vector3 pos);
	bool ClassifyBox(DirectX::BoundingBox box);
};