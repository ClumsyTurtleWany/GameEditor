#pragma once
#include "Define.h"
#include <DirectXCollision.h>

class Camera
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

public:
	void CreateViewMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);
	void CreateProjectionMatrix(float nearDist, float farDist, float fov, float aspectRatio);

public:
	void Update();
};