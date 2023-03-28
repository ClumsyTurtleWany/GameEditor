#include "WeaponMeshComponent.h"
#include <fstream>
#include <sstream>
#include "MaterialManager.h"

WeaponMeshComponent::WeaponMeshComponent()
{
	Initialize();
}
WeaponMeshComponent::~WeaponMeshComponent()
{
}

bool WeaponMeshComponent::Initialize()
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

	PositionBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<TransformMatrix>(PositionData);
	if (PositionBuffer == nullptr)
	{
		OutputDebugString(L"EditorECS::StaticMeshComponent::Initialize::Failed Create Transform Buffer.");
		return false;
	}

	isCreated = true;

	return true;
}

bool WeaponMeshComponent::Render()
{
	DXDevice::g_pImmediateContext->IASetInputLayout(VertexLayout);
	DXDevice::g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DXDevice::g_pImmediateContext->VSSetShader(VertexShader, NULL, 0);
	DXDevice::g_pImmediateContext->HSSetShader(HullShader, NULL, 0);
	DXDevice::g_pImmediateContext->DSSetShader(DomainShader, NULL, 0);
	DXDevice::g_pImmediateContext->GSSetShader(GeometryShader, NULL, 0);
	DXDevice::g_pImmediateContext->UpdateSubresource(PositionBuffer, 0, NULL, &PositionData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(0, 1, &PositionBuffer);

	DXDevice::g_pImmediateContext->PSSetConstantBuffers(4, 1, &PositionBuffer);

	for (auto& it : Meshes)
	{
		it.Render();
	}

	return true;
}

void WeaponMeshComponent::UpdatePos(TransformMatrix pos)
{
	PositionData = pos;
}

bool WeaponMeshComponent::Attach(const SkeletalMeshComponent& mesh, std::string bone)
{
	auto it = mesh.BindPoseKeyToIndexMap.find(bone);
	if (it != mesh.BindPoseKeyToIndexMap.end())
	{
		SocketIndex = it->second;
		return true;
	}
	return false;

}
