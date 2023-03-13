#pragma once
#include "Define.h"
#include "StaticMesh.h"
#include "TransformComponent.h"

class StaticMeshComponent
{
public:
	std::wstring Name;

	std::vector<StaticMesh> Meshes;
	ID3D11InputLayout*		VertexLayout = nullptr;
	ID3D11VertexShader*		VertexShader = nullptr;
	ID3D11HullShader*		HullShader = nullptr;
	ID3D11DomainShader*		DomainShader = nullptr;
	ID3D11GeometryShader*	GeometryShader = nullptr;
	ID3D11Buffer*			TransformBuffer = nullptr;
	TransformMatrix			TransformData;

	bool isCreated = false;

public:
	StaticMeshComponent();
	virtual ~StaticMeshComponent();

public:
	virtual bool Render();
	virtual bool Initialize();

public:
	virtual void UpdateTransformMatrix(const TransformComponent& transform);
};

