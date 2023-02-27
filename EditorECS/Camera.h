#pragma once
#include "Define.h"

class Camera
{
protected:
	ID3D11DeviceContext* Context;

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
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float Roll = 0.0f;

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

public:
	void SetContext(ID3D11DeviceContext* context);
};