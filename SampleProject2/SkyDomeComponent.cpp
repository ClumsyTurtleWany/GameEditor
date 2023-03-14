#include "SkyDomeComponent.h"
#include "FBXLoader.hpp"
#include "MaterialManager.h"

SkyDomeComponent::SkyDomeComponent()
{
	Initialize();
}

SkyDomeComponent::~SkyDomeComponent()
{
}

bool SkyDomeComponent::Initialize()
{
	if (FBXLoader::GetInstance()->Load(L"../resource/FBX/CloudDome.fbx"))
	{
		FBXLoader::GetInstance()->GenerateStaticMeshFromFileData(L"../resource/FBX/CloudDome.fbx", &MeshComp);
	}

	MaterialSlot = new Material;
	MaterialSlot->PixelShaderCodeName = L"Default";
	MaterialSlot->DiffuseTextureName = L"../resource/FBX/CloudSky2.jpg";
	MaterialSlot->Initialize();
	//MaterialManager::GetInstance()->AddMaterial(L"SkyDome", MaterialSlot);

	for (auto& mesh : MeshComp.Meshes)
	{
		mesh.MaterialSlot = MaterialSlot;
	}

	CameraMatrixBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(CameraMatrixData);

	return true;
}

bool SkyDomeComponent::Render()
{
	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::pRSSolid_CullNone);
	MeshComp.Render();
	return true;
}

void SkyDomeComponent::UpdateCamera(Camera* camera)
{
	if (camera == nullptr)
	{
		return;
	}

	DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(Vector3(0.0f, 0.0f, 0.0f));
	MeshComp.TransformData.Mat = DirectX::XMMatrixAffineTransformation(Scale, Vector3(0.0f, 0.0f, 0.0f), q, Vector3(0.0f, 0.0f, 0.0f));
	MeshComp.TransformData.Mat = DirectX::XMMatrixAffineTransformation(Scale, camera->Pos, q, camera->Pos);
	MeshComp.TransformData.InversedMat = DirectX::XMMatrixInverse(0, MeshComp.TransformData.Mat);
	MeshComp.TransformData.Mat = MeshComp.TransformData.Mat.Transpose();

	CameraMatrixData.View = camera->View;
	CameraMatrixData.View._41 = 0.0f;
	CameraMatrixData.View._42 = 0.0f;
	CameraMatrixData.View._43 = 0.0f;
	CameraMatrixData.View = CameraMatrixData.View.Transpose();


	//CameraMatrixData.Projection = DirectX::XMMatrixPerspectiveFovLH(PI * 0.25, 1.0f, 1.0f, 10000.0f);
	CameraMatrixData.Projection = camera->Projection;
	CameraMatrixData.Projection = CameraMatrixData.Projection.Transpose();

	DXDevice::g_pImmediateContext->UpdateSubresource(CameraMatrixBuffer, 0, NULL, &CameraMatrixData, 0, 0);
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &CameraMatrixBuffer);
}

void SkyDomeComponent::SetMaterial(Material* material)
{
	MaterialSlot = material;
	for (auto& mesh : MeshComp.Meshes)
	{
		mesh.MaterialSlot = MaterialSlot;
	}
}
