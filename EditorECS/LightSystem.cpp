#include "LightSystem.h"
#include "World.hpp"
#include "LightComponent.h"
#include "TransformComponent.h"
#include "LandscapeComponents.h"
#include "SkeletalMeshComponent.h"
#include "StaticMeshComponent.h"
#include "SocketComponent.h"
#include "BoundingBoxComponent.h"

void LightSystem::Tick(ECS::World* world, float time)
{
	CleanUp();

	//------------------------------------------------------------------------------
	// Compute Light Data
	//------------------------------------------------------------------------------
	DirectionalLights.Cnt = 0;
	for (auto& entity : world->GetEntities<DirectionalLightComponent>())
	{
		auto lightComp = entity->GetComponent<DirectionalLightComponent>();
		auto transformComp = entity->GetComponent<TransformComponent>();

		DirectionalLights.Color[DirectionalLights.Cnt] = lightComp->Color;
		DirectionalLights.Position[DirectionalLights.Cnt] = Vector4(transformComp->Translation.x, transformComp->Translation.y, transformComp->Translation.z, 0.0f);
		DirectionalLights.Direction[DirectionalLights.Cnt] = lightComp->Direction;
		//DirectionalLights.View[DirectionalLights.Cnt] = DirectX::XMMatrixLookAtLH(DirectionalLights.Position[DirectionalLights.Cnt], DirectionalLights.Direction[DirectionalLights.Cnt], Vector3(0.0f, 1.0f, 0.0f));
		DirectionalLights.Cnt++;
		if (DirectionalLights.Cnt >= MAX_DIRECTIONAL_LIGHT_CNT)
		{
			break;
		}
	}

	PointLights.Cnt = 0;
	for (auto& entity : world->GetEntities<PointLightComponent>())
	{
		auto lightComp = entity->GetComponent<PointLightComponent>();
		auto transformComp = entity->GetComponent<TransformComponent>();

		PointLights.Color[PointLights.Cnt] = lightComp->Color;
		PointLights.Direction[PointLights.Cnt] = lightComp->Direction;
		PointLights.Position[PointLights.Cnt] = Vector4(transformComp->Translation.x, transformComp->Translation.y, transformComp->Translation.z, 0.0f);
		PointLights.Radius[PointLights.Cnt].x = lightComp->Radius;
		//PointLights.View[PointLights.Cnt] = DirectX::XMMatrixLookAtLH(PointLights.Position[PointLights.Cnt], PointLights.Direction[PointLights.Cnt], Vector3(0.0f, 1.0f, 0.0f));
		PointLights.Cnt++;
		if (PointLights.Cnt >= MAX_POINT_LIGHT_CNT)
		{
			break;
		}
	}

	SpotLights.Cnt = 0;
	for (auto& entity : world->GetEntities<SpotLightComponent>())
	{
		auto lightComp = entity->GetComponent<SpotLightComponent>();
		auto transformComp = entity->GetComponent<TransformComponent>();

		SpotLights.Color[SpotLights.Cnt] = lightComp->Color;
		SpotLights.Direction[SpotLights.Cnt] = lightComp->Direction;
		SpotLights.Position[SpotLights.Cnt] = Vector4(transformComp->Translation.x, transformComp->Translation.y, transformComp->Translation.z, 0.0f);
		SpotLights.Radius[SpotLights.Cnt].x = lightComp->Radius;
		SpotLights.Distance[SpotLights.Cnt].x = lightComp->Distance;
		//SpotLights.View[SpotLights.Cnt] = DirectX::XMMatrixLookAtLH(SpotLights.Position[SpotLights.Cnt], SpotLights.Direction[SpotLights.Cnt], Vector3(0.0f, 1.0f, 0.0f));
		SpotLights.Cnt++;
		if (SpotLights.Cnt >= MAX_SPOT_LIGHT_CNT)
		{
			break;
		}
	}

	if (MainCamera != nullptr)
	{
		Eye.Position = Vector4(MainCamera->Pos.x, MainCamera->Pos.y, MainCamera->Pos.z, 0.0f);
		Eye.Direction = Vector4(MainCamera->Look.x, MainCamera->Look.y, MainCamera->Look.z, 0.0f);

		if (DirectionalLightBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(DirectionalLightBuffer, 0, NULL, &DirectionalLights, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(0, 1, &DirectionalLightBuffer);
		}

		if (PointLightBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(PointLightBuffer, 0, NULL, &PointLights, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(1, 1, &PointLightBuffer);
		}

		if (SpotLightBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(SpotLightBuffer, 0, NULL, &SpotLights, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(2, 1, &SpotLightBuffer);
		}

		if (EyeBuffer != nullptr)
		{
			DXDevice::g_pImmediateContext->UpdateSubresource(EyeBuffer, 0, NULL, &Eye, 0, 0);
			DXDevice::g_pImmediateContext->PSSetConstantBuffers(3, 1, &EyeBuffer);
		}
	}

	//------------------------------------------------------------------------------
	// Make Shadow Texture
	//------------------------------------------------------------------------------
	ID3D11ShaderResourceView* pEmptySRV = nullptr;
	DXDevice::g_pImmediateContext->PSSetShaderResources(6, 1, &pEmptySRV);
	DXDevice::g_pImmediateContext->PSSetShaderResources(7, 1, &pEmptySRV);
	DXDevice::g_pImmediateContext->PSSetShaderResources(8, 1, &pEmptySRV);

	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::RSSlopeScaledDepthBias);
	DXDevice::g_pImmediateContext->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);
	DXDevice::g_pImmediateContext->OMSetDepthStencilState(DXSamplerState::pDefaultDepthStencil, 0xff);

	float maxViewDistance = 0.0f;
	for (auto& entity : world->GetEntities<LandscapeComponents, TransformComponent>())
	{
		auto landscape = entity->GetComponent<LandscapeComponents>();
		auto transform = entity->GetComponent<TransformComponent>();
		if ((landscape != nullptr) && (transform != nullptr))
		{
			float mapWidth = landscape->CellDistance * landscape->Column * landscape->SectionSize;
			float mapHeight = landscape->CellDistance * landscape->Row * landscape->SectionSize;
			float dist = sqrt(mapWidth * mapWidth + mapHeight * mapHeight);
			if (dist > maxViewDistance)
			{
				maxViewDistance = dist;
			}
		}
	}

	Matrix projection = DirectX::XMMatrixOrthographicOffCenterLH(-maxViewDistance / 2.0f, maxViewDistance / 2.0f, -maxViewDistance / 2.0f, maxViewDistance / 2.0f, 1.0f, 1000.0f);
	DirectionalLightShadowMatrix.Projection = projection.Transpose();

	//---------------------------------------------------------------------------------------
	// Directional Light Shadow
	//---------------------------------------------------------------------------------------
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &DirectionalLightShadowBuffer);
	for (size_t idx = 0; idx < DirectionalLights.Cnt; idx++)
	{
		Vector4 target = DirectionalLights.Position[idx] + 
			Vector4(fabs(DirectionalLights.Position[idx].x) * DirectionalLights.Direction[idx].x, 
				fabs(DirectionalLights.Position[idx].y) * DirectionalLights.Direction[idx].y, 
				fabs(DirectionalLights.Position[idx].z) * DirectionalLights.Direction[idx].z, 0.0f);
		//DirectionalLightShadowMatrix.View = DirectX::XMMatrixLookAtLH(DirectionalLights.Position[idx], target, Vector3(0.0f, 0.999f, 0.0f));
		DirectionalLightShadowMatrix.View = CreateViewMatrix(DirectionalLights.Position[idx], target, Vector3(0.0f, 1.0f, 0.0f));
		DirectionalLights.View[idx] = DirectionalLightShadowMatrix.View * projection * TextureConversionMatrix;
		DirectionalLights.View[idx] = DirectionalLights.View[idx].Transpose();
		DirectionalLightShadowMatrix.View = DirectionalLightShadowMatrix.View.Transpose();
		DXDevice::g_pImmediateContext->UpdateSubresource(DirectionalLightShadowBuffer, 0, NULL, &DirectionalLightShadowMatrix, 0, 0);

		if (DirectionalLightShadowRenderTarget->Begin(idx))
		{
			for (auto& entity : world->GetEntities<LandscapeComponents, TransformComponent>())
			{
				auto landscape = entity->GetComponent<LandscapeComponents>();
				auto transform = entity->GetComponent<TransformComponent>();
				if ((landscape != nullptr) && (transform != nullptr))
				{
					landscape->SetCamera(MainCamera);
					landscape->UpdateTransformMatrix(*transform);
					landscape->RenderShadow();
				}
			}

			for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
			{
				auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
				auto transform = entity->GetComponent<TransformComponent>();

				if ((skeletalMesh != nullptr) && (transform != nullptr))
				{
					skeletalMesh->UpdateTransformMatrix(*transform);
					skeletalMesh->RenderShadow();
				}

			}

			for (auto& entity : world->GetEntities<StaticMeshComponent, TransformComponent>())
			{
				auto staticMesh = entity->GetComponent<StaticMeshComponent>();
				auto transform = entity->GetComponent<TransformComponent>();
				if ((staticMesh != nullptr) && (transform != nullptr))
				{
					if (entity->has<SocketComponent>())
					{
						auto socket = entity->GetComponent<SocketComponent>();
						staticMesh->UpdateTransformData(socket->GetTransformMatrix());
						staticMesh->RenderShadow();
					}
					else
					{
						staticMesh->UpdateTransformMatrix(*transform);
						staticMesh->RenderShadow();
					}
				}
			}
			DirectionalLightShadowRenderTarget->End(idx);
		}
	}

	//DirectionalLightShadowRenderTarget->RenderDepthTexture(0, 0, 0, 400, 400);
	//DirectionalLightShadowRenderTarget->RenderDepthTexture(1, 0, 400, 400, 400);

	//---------------------------------------------------------------------------------------
	// Point Light Shadow
	//---------------------------------------------------------------------------------------
	projection = DirectX::XMMatrixOrthographicOffCenterLH(-500.0f / 2.0f, 500.0f / 2.0f, -500.0f / 2.0f, 500.0f / 2.0f, 1.0f, 500.0f);
	PointLightShadowMatrix.Projection = projection.Transpose();
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &PointLightShadowBuffer);
	for (size_t idx = 0; idx < PointLights.Cnt; idx++)
	{
		Vector4 target = PointLights.Position[idx] +
			Vector4(fabs(PointLights.Position[idx].x) * PointLights.Direction[idx].x,
				fabs(PointLights.Position[idx].y) * PointLights.Direction[idx].y,
				fabs(PointLights.Position[idx].z) * PointLights.Direction[idx].z, 0.0f);
		//PointLightShadowMatrix.View = DirectX::XMMatrixLookAtLH(PointLights.Position[idx], target, Vector3(0.0f, 0.999f, 0.0f));
		PointLightShadowMatrix.View = CreateViewMatrix(PointLights.Position[idx], target, Vector3(0.0f, 1.0f, 0.0f));
		PointLights.View[idx] = PointLightShadowMatrix.View * projection * TextureConversionMatrix;
		PointLights.View[idx] = PointLights.View[idx].Transpose();
		PointLightShadowMatrix.View = PointLightShadowMatrix.View.Transpose();
		DXDevice::g_pImmediateContext->UpdateSubresource(PointLightShadowBuffer, 0, NULL, &PointLightShadowMatrix, 0, 0);

		if (PointLightShadowRenderTarget->Begin(idx))
		{
			for (auto& entity : world->GetEntities<LandscapeComponents, TransformComponent>())
			{
				auto landscape = entity->GetComponent<LandscapeComponents>();
				auto transform = entity->GetComponent<TransformComponent>();
				if ((landscape != nullptr) && (transform != nullptr))
				{
					landscape->SetCamera(MainCamera);
					landscape->UpdateTransformMatrix(*transform);
					landscape->RenderShadow();
				}
			}

			for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
			{
				auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
				auto transform = entity->GetComponent<TransformComponent>();

				if ((skeletalMesh != nullptr) && (transform != nullptr))
				{
					skeletalMesh->UpdateTransformMatrix(*transform);
					skeletalMesh->RenderShadow();
				}
			}

			for (auto& entity : world->GetEntities<StaticMeshComponent, TransformComponent>())
			{
				auto staticMesh = entity->GetComponent<StaticMeshComponent>();
				auto transform = entity->GetComponent<TransformComponent>();
				if ((staticMesh != nullptr) && (transform != nullptr))
				{
					if (entity->has<SocketComponent>())
					{
						auto socket = entity->GetComponent<SocketComponent>();
						staticMesh->UpdateTransformData(socket->GetTransformMatrix());
						staticMesh->RenderShadow();
					}
					else
					{
						staticMesh->UpdateTransformMatrix(*transform);
						staticMesh->RenderShadow();
					}
				}
			}
			PointLightShadowRenderTarget->End(idx);
		}
	}

	//---------------------------------------------------------------------------------------
	// Spot Light Shadow
	//---------------------------------------------------------------------------------------
	projection = DirectX::XMMatrixOrthographicOffCenterLH(-500.0f / 2.0f, 500.0f / 2.0f, -500.0f / 2.0f, 500.0f / 2.0f, 1.0f, 500.0f);
	SpotLightShadowMatrix.Projection = projection.Transpose();
	DXDevice::g_pImmediateContext->VSSetConstantBuffers(1, 1, &SpotLightShadowBuffer);
	for (size_t idx = 0; idx < SpotLights.Cnt; idx++)
	{
		Vector4 target = SpotLights.Position[idx] +
			Vector4(fabs(SpotLights.Position[idx].x) * SpotLights.Direction[idx].x,
				fabs(SpotLights.Position[idx].y) * SpotLights.Direction[idx].y,
				fabs(SpotLights.Position[idx].z) * SpotLights.Direction[idx].z, 0.0f);
		//SpotLightShadowMatrix.View = DirectX::XMMatrixLookAtLH(SpotLights.Position[idx], target, Vector3(0.0f, 0.0f, 1.0f));
		SpotLightShadowMatrix.View = CreateViewMatrix(SpotLights.Position[idx], target, Vector3(0.0f, 1.0f, 0.0f));
		SpotLights.View[idx] = SpotLightShadowMatrix.View * projection * TextureConversionMatrix;
		SpotLights.View[idx] = SpotLights.View[idx].Transpose();
		SpotLightShadowMatrix.View = SpotLightShadowMatrix.View.Transpose();
		DXDevice::g_pImmediateContext->UpdateSubresource(SpotLightShadowBuffer, 0, NULL, &SpotLightShadowMatrix, 0, 0);

		if (SpotLightShadowRenderTarget->Begin(idx))
		{
			for (auto& entity : world->GetEntities<LandscapeComponents, TransformComponent>())
			{
				auto landscape = entity->GetComponent<LandscapeComponents>();
				auto transform = entity->GetComponent<TransformComponent>();
				if ((landscape != nullptr) && (transform != nullptr))
				{
					landscape->SetCamera(MainCamera);
					landscape->UpdateTransformMatrix(*transform);
					landscape->RenderShadow();
				}
			}

			for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
			{
				auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
				auto transform = entity->GetComponent<TransformComponent>();
				if ((skeletalMesh != nullptr) && (transform != nullptr))
				{
					skeletalMesh->UpdateTransformMatrix(*transform);
					skeletalMesh->RenderShadow();
				}
			}

			for (auto& entity : world->GetEntities<StaticMeshComponent, TransformComponent>())
			{
				auto staticMesh = entity->GetComponent<StaticMeshComponent>();
				auto transform = entity->GetComponent<TransformComponent>();
				if ((staticMesh != nullptr) && (transform != nullptr))
				{
					if (entity->has<SocketComponent>())
					{
						auto socket = entity->GetComponent<SocketComponent>();
						staticMesh->UpdateTransformData(socket->GetTransformMatrix());
						staticMesh->RenderShadow();
					}
					else
					{
						staticMesh->UpdateTransformMatrix(*transform);
						staticMesh->RenderShadow();
					}
				}
			}
			SpotLightShadowRenderTarget->End(idx);
		}
	}

	DXDevice::g_pImmediateContext->PSSetShaderResources(6, 1, &DirectionalLightShadowRenderTarget->DSVShaderResourceView);
	DXDevice::g_pImmediateContext->PSSetShaderResources(7, 1, &PointLightShadowRenderTarget->DSVShaderResourceView);
	DXDevice::g_pImmediateContext->PSSetShaderResources(8, 1, &SpotLightShadowRenderTarget->DSVShaderResourceView);

	if (DirectionalLightBuffer != nullptr)
	{
		DXDevice::g_pImmediateContext->UpdateSubresource(DirectionalLightBuffer, 0, NULL, &DirectionalLights, 0, 0);
		DXDevice::g_pImmediateContext->PSSetConstantBuffers(0, 1, &DirectionalLightBuffer);
	}

	if (PointLightBuffer != nullptr)
	{
		DXDevice::g_pImmediateContext->UpdateSubresource(PointLightBuffer, 0, NULL, &PointLights, 0, 0);
		DXDevice::g_pImmediateContext->PSSetConstantBuffers(1, 1, &PointLightBuffer);
	}

	if (SpotLightBuffer != nullptr)
	{
		DXDevice::g_pImmediateContext->UpdateSubresource(SpotLightBuffer, 0, NULL, &SpotLights, 0, 0);
		DXDevice::g_pImmediateContext->PSSetConstantBuffers(2, 1, &SpotLightBuffer);
	}
}

void LightSystem::Initialize()
{
	DirectionalLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<DirectionalLightData>(DirectionalLights);
	PointLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<PointLightData>(PointLights);
	SpotLightBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<SpotLightData>(SpotLights);
	EyeBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<EyeData>(Eye);

	// Shadow
	DirectionalLights.ShadowTextureSize = 4096.0f;
	PointLights.ShadowTextureSize = 1024.0f;
	SpotLights.ShadowTextureSize = 1024.0f;

	DirectionalLightShadowRenderTarget = DXRenderTargetManager::GetInstance()->Create(0, 0, DirectionalLights.ShadowTextureSize, DirectionalLights.ShadowTextureSize, MAX_DIRECTIONAL_LIGHT_CNT);
	PointLightShadowRenderTarget = DXRenderTargetManager::GetInstance()->Create(0, 0, PointLights.ShadowTextureSize, PointLights.ShadowTextureSize, MAX_POINT_LIGHT_CNT);
	SpotLightShadowRenderTarget = DXRenderTargetManager::GetInstance()->Create(0, 0, SpotLights.ShadowTextureSize, SpotLights.ShadowTextureSize, MAX_SPOT_LIGHT_CNT);

	DirectionalLightShadowBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(DirectionalLightShadowMatrix);
	PointLightShadowBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(PointLightShadowMatrix);
	SpotLightShadowBuffer = DXShaderManager::GetInstance()->CreateConstantBuffer<CameraMatrix>(SpotLightShadowMatrix);

	TextureConversionMatrix = Matrix( 0.5f, 0.0f, 0.0f, 0.0f
									, 0.0f, -0.5f, 0.0f, 0.0f
									, 0.0f, 0.0f, 1.0f, 0.0f
									, 0.5f, 0.5f, 0.0f, 1.0f);
}

void LightSystem::CleanUp()
{
	for (int idx = 0; idx < MAX_DIRECTIONAL_LIGHT_CNT; idx++)
	{
		DirectionalLights.Color[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		DirectionalLights.Position[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		DirectionalLights.Direction[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		//DirectionalLights.View[idx] = Matrix();
	}

	for (int idx = 0; idx < MAX_POINT_LIGHT_CNT; idx++)
	{	
		PointLights.Color[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights.Position[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights.Direction[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		PointLights.Radius[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		//PointLights.View[idx] = Matrix();
	}

	for (int idx = 0; idx < MAX_SPOT_LIGHT_CNT; idx++)
	{
		SpotLights.Color[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		SpotLights.Position[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		SpotLights.Direction[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		SpotLights.Radius[idx] = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		//SpotLights.View[idx] = Matrix();
	}

	DirectionalLights.Cnt = 0;
	SpotLights.Cnt = 0;
	PointLights.Cnt = 0;
	Eye.Position = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Eye.Direction = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
}

Matrix LightSystem::CreateViewMatrix(const Vector4& eye, const Vector4& target, const Vector3& up)
{
	Matrix view;

	Vector3 direction = Vector3(target.x, target.y, target.z);
	direction -= Vector3(eye.x, eye.y, eye.z);
	direction.Normalize();
	Vector3 diff = direction + up;
	Vector3 newUp = up;
	if (diff.x < EPSILON && diff.y < EPSILON && diff.z < EPSILON)
	{
		newUp = Vector3(0.0f, 0.0f, -1.0f);
	}
	Vector3 right = newUp.Cross(direction);
	right.Normalize();
	newUp = direction.Cross(right);
	newUp.Normalize();

	view._11 = right.x; view._12 = newUp.x; view._13 = direction.x;
	view._21 = right.y; view._22 = newUp.y; view._23 = direction.y;
	view._31 = right.z; view._32 = newUp.z; view._33 = direction.z;
	view._41 = -(eye.x * view._11 + eye.y * view._21 + eye.z * view._31);
	view._42 = -(eye.x * view._12 + eye.y * view._22 + eye.z * view._32);
	view._43 = -(eye.x * view._13 + eye.y * view._23 + eye.z * view._33);
	view._14 = view._24 = view._34 = 0.0f;
	view._44 = 1.0f;

	return view;
}