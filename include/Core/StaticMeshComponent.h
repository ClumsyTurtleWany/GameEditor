#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "Material.h"
#include "MeshComponent.hpp"
#include "DXShaderManager.h"

class StaticMeshComponent
{
public:
	std::vector<MeshComponent> Meshes;
	ID3D11InputLayout*		VertexLayout = nullptr;
	ID3D11VertexShader*		VertexShader = nullptr;
	ID3D11HullShader*		HullShader = nullptr;
	ID3D11DomainShader*		DomainShader = nullptr;
	ID3D11GeometryShader*	GeometryShader = nullptr;
	ID3D11Buffer*			TransformBuffer = nullptr;
	VertexTransform			TransformData;

	bool isCreated = false;

public:
	StaticMeshComponent() {};

public:
	bool Render();
	bool Initialize();
};

inline bool StaticMeshComponent::Render()
{
	if (!isCreated)
	{
		Initialize();
	}

	DEVICE->GetContext()->IASetInputLayout(VertexLayout);
	DEVICE->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DEVICE->GetContext()->VSSetShader(VertexShader, NULL, 0);
	DEVICE->GetContext()->HSSetShader(HullShader, NULL, 0);
	DEVICE->GetContext()->DSSetShader(DomainShader, NULL, 0);
	DEVICE->GetContext()->GSSetShader(GeometryShader, NULL, 0);
	DEVICE->GetContext()->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	
	for (auto& it : Meshes)
	{
		it.Render();
	}

	return true;
}

inline bool StaticMeshComponent::Initialize()
{
	if (isCreated)
	{
		return true;
	}

	VertexLayout = DXShaderManager::getInstance()->GetInputLayout(InputLayoutType::StaticMesh);
	VertexShader = DXShaderManager::getInstance()->GetVertexShader(L"../include/Core/HLSL/VS_StaticMesh.hlsl");
	TransformBuffer = DXShaderManager::getInstance()->CreateConstantBuffer<VertexTransform>(TransformData);
	isCreated = true;

	return true;
}
