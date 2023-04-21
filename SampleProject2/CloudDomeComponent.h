#pragma once
#include "StaticMeshComponent.h"
#include "Camera.h"
#include "TransformComponent.h"

class CloudDomeComponent
{
public:
	StaticMeshComponent MeshComp;
	Material* MaterialSlot = nullptr;
	ID3D11Buffer* CameraMatrixBuffer = nullptr;
	CameraMatrix  CameraMatrixData;
	Vector3 Scale = Vector3(10.0f, 10.0f, 10.0f);
	float RotationSpeed = 0.1f;

public:
	CloudDomeComponent();
	~CloudDomeComponent();

public:
	bool Initialize();
	bool Render();

public:
	virtual void UpdateTransformMatrix(const TransformComponent& transform);
	virtual void UpdateCamera(Camera* camera);
	void SetMaterial(Material* material);
};