#include "Camera.h"
#include "DXShaderManager.h"
#include "DXDevice.hpp"

Camera::Camera()
{
	CameraMatrixBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(CameraMatrixData);
}

Camera::~Camera()
{
}

void Camera::CreateViewMatrix(const Vector3& eye, const Vector3& target, const Vector3& up)
{
	Pos = eye;
	Look = target;
	Up = up;

	Vector3 direction = target;
	direction -= eye;
	direction.Normalize();
	Vector3 right = up.Cross(direction);
	right.Normalize();
	Vector3 newUp = direction.Cross(right);
	newUp.Normalize();

	View._11 = right.x; View._12 = newUp.x; View._13 = direction.x;
	View._21 = right.y; View._22 = newUp.y; View._23 = direction.y;
	View._31 = right.z; View._32 = newUp.z; View._33 = direction.z;
	View._41 = -(Pos.x * View._11 + Pos.y * View._21 + Pos.z * View._31);
	View._42 = -(Pos.x * View._12 + Pos.y * View._22 + Pos.z * View._32);
	View._43 = -(Pos.x * View._13 + Pos.y * View._23 + Pos.z * View._33);
	View._14 = View._24 = View._34 = 0.0f;
	View._44 = 1.0f;
	
}

void Camera::CreateProjectionMatrix(float nearDist, float farDist, float fov, float aspectRatio)
{
	NearDistance = nearDist;
	FarDistance = farDist;
	FovY = fov;
	AspectRatio = aspectRatio;

	float h = 1.0f / tan(fov * 0.5f);
	float w = h / AspectRatio;
	float q = FarDistance / (FarDistance - NearDistance);

	/*Projection._11 = w;
	Projection._22 = h;
	Projection._33 = q;
	Projection._43 = -q * NearDistance;
	Projection._34 = 1.0f;
	Projection._44 = 0.0f;*/

	Projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, NearDistance, FarDistance);

	Frustum = DirectX::BoundingFrustum(Projection);
}

void Camera::Update()
{
	Right.x = View._11;
	Right.y = View._21;
	Right.z = View._31;

	Up.x = View._12;
	Up.y = View._22;
	Up.z = View._32;

	Look.x = View._13;
	Look.y = View._23;
	Look.z = View._33;

	Right.Normalize();
	Up.Normalize();
	Look.Normalize();

	CameraMatrixData.View = View.Transpose();
	CameraMatrixData.Projection = Projection.Transpose();

	DXDevice::g_pImmediateContext->UpdateSubresource(CameraMatrixBuffer, 0, NULL, &CameraMatrixData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);
}