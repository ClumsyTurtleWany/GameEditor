#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "Material.h"
#include "MeshComponent.hpp"
#include "DXShaderManager.h"
#include "TransformComponent.h"

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
	TransformMatrix			TransformData;

	bool isCreated = false;

public:
	StaticMeshComponent() { Initialize(); };

public:
	bool Render();
	bool Initialize();

public:
	void UpdateTransformMatrix(const TransformComponent& transform);
};

inline bool StaticMeshComponent::Render()
{
	/*if (!isCreated)
	{
		Initialize();
	}*/

	CONTEXT->IASetInputLayout(VertexLayout);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CONTEXT->VSSetShader(VertexShader, NULL, 0);
	CONTEXT->HSSetShader(HullShader, NULL, 0);
	CONTEXT->DSSetShader(DomainShader, NULL, 0);
	CONTEXT->GSSetShader(GeometryShader, NULL, 0);
	CONTEXT->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	CONTEXT->VSSetConstantBuffers(0, 1, &TransformBuffer);
	
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
	TransformBuffer = DXShaderManager::getInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	isCreated = true;

	return true;
}

inline void StaticMeshComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.Mat = TransformData.Mat.Transpose();
}
