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

public:
	StaticMeshComponent() {};

public:
	bool Render();
};

bool StaticMeshComponent::Render()
{
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
}