#pragma once
#include "Define.h"
#include "System.hpp"
#include "World.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "DXSamplerState.hpp"

class RenderSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		ID3D11RasterizerState* pOldRSState;
		CONTEXT->RSGetState(&pOldRSState);
		CONTEXT->RSSetState(DXSamplerState::pDefaultRSSolid);
		CONTEXT->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);
		CONTEXT->PSSetSamplers(0, 1, &DXSamplerState::pDefaultSamplerState);
		for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
		{
			auto skeletalMesh = entity.get()->GetComponent<SkeletalMeshComponent>();
			
		}

		for (auto& entity : world->GetEntities<StaticMeshComponent>())
		{
			auto staticMesh = entity.get()->GetComponent<StaticMeshComponent>();
			staticMesh->Render();
		}

		CONTEXT->RSSetState(pOldRSState);
		if (pOldRSState != nullptr)
		{
			pOldRSState->Release();
		}
	}
};