#include "RenderSystem.h"
//#include "Define.h"
#include "World.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "DXSamplerState.hpp"
#include "TransformComponent.h"
#include "Landscape.h"
#include "DebugCamera.h"

RenderSystem::RenderSystem()
{
	Context = DXShaderManager::getInstance()->GetContext();
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Tick(ECS::World* world, float time)
{
	ID3D11RasterizerState* pOldRSState;
	Context->RSGetState(&pOldRSState);
	Context->RSSetState(DXSamplerState::pDefaultRSSolid);
	Context->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);
	Context->PSSetSamplers(0, 1, &DXSamplerState::pDefaultSamplerState);

	for (auto& entity : world->GetEntities<Landscape>())
	{
		auto landscape = entity->GetComponent<Landscape>();
		auto transform = entity->GetComponent<TransformComponent>();
		if ((landscape != nullptr) && (transform != nullptr))
		{
			landscape->UpdateTransformMatrix(*transform);
			landscape->Render();
		}
	}

	for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
	{
		auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();

	}

	for (auto& entity : world->GetEntities<StaticMeshComponent>())
	{
		auto staticMesh = entity->GetComponent<StaticMeshComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		
		if ((staticMesh != nullptr) && (transform != nullptr))
		{
			staticMesh->UpdateTransformMatrix(*transform);
			staticMesh->Render();
		}
	}

	Context->RSSetState(pOldRSState);
	if (pOldRSState != nullptr)
	{
		pOldRSState->Release();
	}
}

void RenderSystem::SetContext(ID3D11DeviceContext* context)
{
	Context = context;
}
