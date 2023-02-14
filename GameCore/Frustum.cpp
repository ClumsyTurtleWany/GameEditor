#include "Frustum.h"

bool Frustum::CreateFrustum(Matrix view, Matrix proj)
{
	FrustumVertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	FrustumVertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	FrustumVertices[2] = Vector3(1.0f, 1.0f, 0.0f);
	FrustumVertices[3] = Vector3(1.0f, -1.0f, 0.0f);
	FrustumVertices[4] = Vector3(-1.0f, -1.0f, 1.0f);
	FrustumVertices[5] = Vector3(-1.0f, 1.0f, 1.0f);
	FrustumVertices[6] = Vector3(1.0f, 1.0f, 1.0f);
	FrustumVertices[7] = Vector3(1.0f, -1.0f, 1.0f);

	Matrix ViewCompProj = view * proj;
	Matrix InversedMatrix = DirectX::XMMatrixInverse(nullptr, ViewCompProj);
	for (int idx = 0; idx < 8; idx++)
	{
		float x = FrustumVertices[idx].x * InversedMatrix._11 + FrustumVertices[idx].y * InversedMatrix._21 + FrustumVertices[idx].z * InversedMatrix._31 + 1.0f * InversedMatrix._41;
		float y = FrustumVertices[idx].x * InversedMatrix._12 + FrustumVertices[idx].y * InversedMatrix._22 + FrustumVertices[idx].z * InversedMatrix._32 + 1.0f * InversedMatrix._42;
		float z = FrustumVertices[idx].x * InversedMatrix._13 + FrustumVertices[idx].y * InversedMatrix._23 + FrustumVertices[idx].z * InversedMatrix._33 + 1.0f * InversedMatrix._43;
		float w = FrustumVertices[idx].x * InversedMatrix._14 + FrustumVertices[idx].y * InversedMatrix._24 + FrustumVertices[idx].z * InversedMatrix._34 + 1.0f * InversedMatrix._44;

		FrustumVertices[idx].x = x / w;
		FrustumVertices[idx].y = y / w;
		FrustumVertices[idx].z = z / w;
	}

	// Front
	FrustumPlane[0] = Plane(FrustumVertices[2], FrustumVertices[1], FrustumVertices[0]);
	// Left
	FrustumPlane[1] = Plane(FrustumVertices[5], FrustumVertices[4], FrustumVertices[1]);
	// Right
	FrustumPlane[2] = Plane(FrustumVertices[7], FrustumVertices[6], FrustumVertices[2]);
	// Back
	FrustumPlane[3] = Plane(FrustumVertices[4], FrustumVertices[5], FrustumVertices[6]);
	// Top
	FrustumPlane[4] = Plane(FrustumVertices[6], FrustumVertices[5], FrustumVertices[1]);
	// Bottom
	FrustumPlane[5] = Plane(FrustumVertices[4], FrustumVertices[7], FrustumVertices[0]);

	return true;
}

bool Frustum::ClassifyPoint(Vector3 pos)
{
	for (int idx = 0; idx < 6; idx++)
	{
		Vector3 normal = Vector3(FrustumPlane[idx].x, FrustumPlane[idx].y, FrustumPlane[idx].z);
		float h = normal.Dot(pos) + FrustumPlane[idx].w;
		if (h < 0)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::ClassifyBox(DirectX::BoundingBox box)
{
	bool isIntersect = false;
	for (int idx = 0; idx < 6; idx++)
	{
		
		DirectX::PlaneIntersectionType IntersectType = box.intersects(FrustumPlane[idx]);
		box.Intersects(,)
	}
	return false;
}
