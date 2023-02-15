#include "RenderSystem.h"
#include "Define.h"
#include "World.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "DXSamplerState.hpp"
#include "TransformComponent.h"

void RenderSystem::Tick(ECS::World* world, float time)
{
	ID3D11RasterizerState* pOldRSState;
	CONTEXT->RSGetState(&pOldRSState);
	CONTEXT->RSSetState(DXSamplerState::pDefaultRSSolid);
	CONTEXT->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);
	CONTEXT->PSSetSamplers(0, 1, &DXSamplerState::pDefaultSamplerState);
	
	/*DebugCamera* camera = world->GetDebugCamera();
	if (camera != nullptr)
	{
		CONTEXT->UpdateSubresource(camera->CameraMatrixBuffer, 0, NULL, &camera->CameraMatrixData, 0, 0);
	}*/



	for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
	{
		auto skeletalMesh = entity.get()->GetComponent<SkeletalMeshComponent>();

	}

	for (auto& entity : world->GetEntities<StaticMeshComponent>())
	{
		auto staticMesh = entity.get()->GetComponent<StaticMeshComponent>();
		auto transform = entity.get()->GetComponent<TransformComponent>();
		
		if ((staticMesh != nullptr) && (transform != nullptr))
		{
			staticMesh->UpdateTransformMatrix(*transform);
			staticMesh->Render();
		}
	}

	CONTEXT->RSSetState(pOldRSState);
	if (pOldRSState != nullptr)
	{
		pOldRSState->Release();
	}
}