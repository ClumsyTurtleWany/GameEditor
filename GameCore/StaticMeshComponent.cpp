#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent()
{
	Context = DXShaderManager::getInstance()->GetContext();
	Initialize();
}
StaticMeshComponent::~StaticMeshComponent()
{
}
;

bool StaticMeshComponent::Render()
{
	/*if (!isCreated)
	{
		Initialize();
	}*/

	Context->IASetInputLayout(VertexLayout);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Context->VSSetShader(VertexShader, NULL, 0);
	Context->HSSetShader(HullShader, NULL, 0);
	Context->DSSetShader(DomainShader, NULL, 0);
	Context->GSSetShader(GeometryShader, NULL, 0);
	Context->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	Context->VSSetConstantBuffers(0, 1, &TransformBuffer);

	for (auto& it : Meshes)
	{
		it.Render();
	}

	return true;
}

bool StaticMeshComponent::Initialize()
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

void StaticMeshComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.Mat = TransformData.Mat.Transpose();
}

void StaticMeshComponent::SetContext(ID3D11DeviceContext* context)
{
	Context = context;
}
