#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "Material.h"
#include "MeshComponent.hpp"
#include "DXShaderManager.h"
#include "TransformComponent.h"

class StaticMeshComponent
{
private:
	ID3D11DeviceContext* Context;

public:
	std::vector<MeshComponent> Meshes;
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
	~StaticMeshComponent();

public:
	bool Render();
	bool Initialize();

public:
	void UpdateTransformMatrix(const TransformComponent& transform);
	void SetContext(ID3D11DeviceContext* context);
};

