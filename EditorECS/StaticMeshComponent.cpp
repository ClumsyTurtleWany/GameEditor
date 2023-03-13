#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent()
{
	Initialize();
}
StaticMeshComponent::~StaticMeshComponent()
{
}

bool StaticMeshComponent::Render()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(TransformBuffer, 0, NULL, &TransformData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &TransformBuffer);

	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &TransformBuffer);

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
	if (VertexLayout == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Get Input Layout.");
		return false;
	}

	VertexShader = DXShaderManager::GetInstance()->GetVertexShader(L"StaticMesh");
	if (VertexShader == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Get Vertex Shader.");
		return false;
	}

	TransformBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(TransformData);
	if (TransformBuffer == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Create Transform Buffer.");
		return false;
	}

	isCreated = true;

	return true;
}

void StaticMeshComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(transform.Rotation);
	TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	TransformData.InversedMat = DirectX::XMMatrixInverse(0, TransformData.Mat);
	TransformData.Mat = TransformData.Mat.Transpose();
}