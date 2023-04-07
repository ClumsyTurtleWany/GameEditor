#include "WidgetRenderSystem.h"
#include "World.hpp"
#include "DXSamplerState.hpp"
#include "TransformComponent.h"
#include "WidgetComponent.h"

WidgetRenderSystem::WidgetRenderSystem()
{
	Context = DXDevice::g_pImmediateContext;
}

WidgetRenderSystem::~WidgetRenderSystem()
{
}

void WidgetRenderSystem::Tick(ECS::World* world, float time)
{
	ID3D11RasterizerState* pOldRSState;
	Context->RSGetState(&pOldRSState);
	Context->RSSetState(DXSamplerState::pDefaultRSSolid);
	Context->OMSetBlendState(DXSamplerState::pBlendSamplerState, 0, -1);
	Context->PSSetSamplers(0, 1, &DXSamplerState::pDefaultMirrorSamplerState);

	for (auto& entity : world->GetEntities<WidgetComponent>()) 
	{
		auto widget = entity->GetComponent<WidgetComponent>();
		
		if (widget != nullptr) 
		{
			widget->Render(); 
		}
	}

	Context->RSSetState(pOldRSState);
	if (pOldRSState != nullptr)
	{
		pOldRSState->Release();
	}
}