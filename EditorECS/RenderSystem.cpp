#include "RenderSystem.h"
//#include "Define.h"
#include "World.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "DXSamplerState.hpp"
#include "TransformComponent.h"
#include "Landscape.h"
//#include "DebugCamera.h"
#include "SkyBoxComponent.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"
#include "EffectInclude/EffectSystem.h"

void RenderSystem::Tick(ECS::World* world, float time)
{
	ID3D11RasterizerState* pOldRSState;
	DXDevice::g_pImmediateContext->RSGetState(&pOldRSState);
	DXDevice::g_pImmediateContext->RSSetState(DXSamplerState::pDefaultRSSolid);
	DXDevice::g_pImmediateContext->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);
	DXDevice::g_pImmediateContext->PSSetSamplers(0, 1, &DXSamplerState::pDefaultSamplerState);
	DXDevice::g_pImmediateContext->OMSetDepthStencilState(DXSamplerState::pDefaultDepthStencil, 0xff);

	for (auto& entity : world->GetEntities<LandscapeComponents, TransformComponent>())
	{
		auto landscape = entity->GetComponent<LandscapeComponents>();
		auto transform = entity->GetComponent<TransformComponent>();
		if ((landscape != nullptr) && (transform != nullptr))
		{
			landscape->SetCamera(MainCamera);
			landscape->UpdateTransformMatrix(*transform);
			landscape->Render();
		}
	}

	for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
	{
		auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
		auto transform = entity->GetComponent<TransformComponent>();

		if ((skeletalMesh != nullptr) && (transform != nullptr))
		{
			skeletalMesh->UpdateTransformMatrix(*transform);
			skeletalMesh->Render();
		}

	}

	for (auto& entity : world->GetEntities<StaticMeshComponent, TransformComponent>())
	{
		auto staticMesh = entity->GetComponent<StaticMeshComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		
		if ((staticMesh != nullptr) && (transform != nullptr))
		{
			staticMesh->UpdateTransformMatrix(*transform);
			staticMesh->Render();
		}
	}

	for (auto& entity : world->GetEntities<SkyBoxComponent>())
	{
		auto skyBox = entity->GetComponent<SkyBoxComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		if (MainCamera != nullptr)
		{
			skyBox->UpdateCamera(MainCamera, *transform);
		}

		if ((skyBox != nullptr))
		{
			skyBox->Render();
		}
	}

//바운딩 볼륨 렌더 테스트용 코드
#ifdef _DEBUG
	for (auto& entity : world->GetEntities<BoundingBoxComponent>())
	{
		auto BVolume = entity->GetComponent<BoundingBoxComponent>();

		if (BVolume != nullptr && MainCamera != nullptr)
		{
			if (BVolume->pDebugObj)
			{
				BVolume->pDebugObj->update(nullptr, &MainCamera->View, &MainCamera->Projection);
				BVolume->pDebugObj->render();
			}
		}
	}

	for (auto& entity : world->GetEntities<BoundingSphereComponent>())
	{
		auto BVolume = entity->GetComponent<BoundingSphereComponent>();

		if (BVolume != nullptr && MainCamera != nullptr)
		{
			if (BVolume->pDebugObj)
			{
				BVolume->pDebugObj->update(nullptr, &MainCamera->View, &MainCamera->Projection);
				BVolume->pDebugObj->render();
			}
		}
	}
#endif //_DEBUG

	for (auto& entity : world->GetEntities<ParticleEffectComponent>())
	{
		auto pPSystem = entity->GetComponent<ParticleEffectComponent>()->m_pPPsystem;
		auto transform = entity->GetComponent<TransformComponent>();

		Matrix retMat = Matrix::CreateScale(transform->Scale);
		retMat *= Matrix::CreateFromYawPitchRoll(transform->Rotation);
		retMat *= Matrix::CreateTranslation(transform->Translation);

		if (MainCamera != nullptr && pPSystem != nullptr)
		{
			if (pPSystem->m_bPendingDelete)
			{
				world->emit<ECS::EFFECTEVENT_TIMEOUT>({ entity });
			}
			else
			{
				pPSystem->setDevice(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
				pPSystem->update();
				pPSystem->preRender(&retMat,
					&MainCamera->View, &MainCamera->Projection);
				pPSystem->render();
			}
		}
	}

	DXDevice::g_pImmediateContext->GSSetShader(NULL, NULL, 0);

	DXDevice::g_pImmediateContext->RSSetState(pOldRSState);
	if (pOldRSState != nullptr)
	{
		pOldRSState->Release();
	}
}