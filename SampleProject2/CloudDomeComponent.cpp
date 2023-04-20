#include "CloudDomeComponent.h"
#include "FBXLoader.hpp"
#include "MaterialManager.h"

CloudDomeComponent::CloudDomeComponent()
{
	Initialize();
}

CloudDomeComponent::~CloudDomeComponent()
{
}

bool CloudDomeComponent::Initialize()
{
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/CloudDome.fbx"))
	{
		FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/CloudDome.fbx", &MeshComp);
	}

	if (!MaterialManager::GetInstance()->Load(L"../resource/Material/CloudDome.material", L"CloudDome"))
	{
		MaterialSlot = MaterialManager::GetInstance()->CreateMaterial(L"CloudDome");
		MaterialSlot->Name = L"CloudDome";
		MaterialSlot->Type = MaterialType::Sky;
		MaterialSlot->DiffuseTextureName = L"../resource/Texture/CloudSky.png";
		if (MaterialSlot->Create())
		{
			MaterialSlot->Save(L"../resource/Material/CloudDome.material");
		}
		else
		{
			delete MaterialSlot;
			OutputDebugString(L"CloudDomeComponent::Failed Initailize Material.");
			return false;
		}
	}
	else
	{
		MaterialSlot = MaterialManager::GetInstance()->GetMaterial(L"CloudDome");
	}

	for (auto& mesh : MeshComp.Meshes)
	{
		mesh.MaterialSlot = MaterialSlot;
	}

	CameraMatrixBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(CameraMatrixData);

	return true;
}

bool CloudDomeComponent::Render()
{
	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::pRSSolid_CullNone);
	MeshComp.Render();
	return true;
}

void CloudDomeComponent::UpdateTransformMatrix(const TransformComponent& transform)
{
	Vector3 rotation = transform.Rotation / 180.0f * PI;
	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
	//TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, transform.Translation, q, transform.Translation);
	MeshComp.TransformData.Mat = DirectX::XMMatrixAffineTransformation(transform.Scale, Vector3(0.0f, 0.0f, 0.0f), q, transform.Translation);
	MeshComp.TransformData.InversedMat = DirectX::XMMatrixInverse(0, MeshComp.TransformData.Mat);
	MeshComp.TransformData.Mat = MeshComp.TransformData.Mat.Transpose();
}

void CloudDomeComponent::UpdateCamera(Camera* camera)
{
	if (camera == nullptr)
	{
		return;
	}

	CameraMatrixData.View = camera->View;
	//CameraMatrixData.View._41 = 0.0f;
	//CameraMatrixData.View._42 = 0.0f;
	//CameraMatrixData.View._43 = 0.0f;
	CameraMatrixData.View = CameraMatrixData.View.Transpose();

	CameraMatrixData.Projection = camera->Projection;
	CameraMatrixData.Projection = CameraMatrixData.Projection.Transpose();

	DXDevice::g_pImmediateContext->UpdateSubresource(CameraMatrixBuffer, 0, NULL, &CameraMatrixData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);
}

void CloudDomeComponent::SetMaterial(Material* material)
{
	MaterialSlot = material;
	for (auto& mesh : MeshComp.Meshes)
	{
		mesh.MaterialSlot = MaterialSlot;
	}
}
