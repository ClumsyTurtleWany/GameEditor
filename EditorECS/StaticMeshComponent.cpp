#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent()
{
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

	DXDevice::m_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::m_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::m_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::m_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::m_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::m_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::m_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

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

	VertexLayout = DXShaderManager::GetInstance()->GetInputLayout(L"StaticMesh");
	VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"StaticMesh");
	TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	isCreated = true;

	return true;
}

void StaticMeshComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.Mat = TransformData.Mat.Transpose();
}