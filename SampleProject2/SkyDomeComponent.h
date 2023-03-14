#pragma once
#include "StaticMeshComponent.h"
#include "Camera.h"

class SkyDomeComponent
{
public:
	StaticMeshComponent MeshComp;
	Material* MaterialSlot = nullptr;
	ID3D11Buffer* CameraMatrixBuffer = nullptr;
	CameraMatrix  CameraMatrixData;
	Vector3 Scale = Vector3(10.0f, 10.0f, 10.0f);

public:
	SkyDomeComponent();
	~SkyDomeComponent();

public:
	bool Initialize();
	bool Render();
	
public:
	virtual void UpdateCamera(Camera* camera);
	void SetMaterial(Material* material);
};