#pragma once
#include "Define.h"

class Camera
{
public:
	DirectX::BoundingFrustum Frustum;

public:
	Vector3 Pos;
	Vector3 Look;
	Vector3 Right;
	Vector3 Up;

public:
	Matrix View;
	Matrix Projection;

public:
	float FarDistance;
	float NearDistance;
	float AspectRatio;
	float FovX;
	float FovY;

public:
	float Pitch = 0.0f;		//Right Vector		(X Rotation)
	float Yaw = 0.0f;		//Up Vector			(Y Rotation)
	float Roll = 0.0f;		//Forward Vector	(Z Rotation)

	Quaternion	lastQ;		//트레이스 보간을 위한 쿼터니언

public:
	CameraMatrix CameraMatrixData;
	ID3D11Buffer* CameraMatrixBuffer = nullptr;

public:
	Camera();
	virtual ~Camera();

public:
	void CreateViewMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);
	void CreateProjectionMatrix(float nearDist, float farDist, float fov, float aspectRatio);
	void Update();
	void Apply();
};