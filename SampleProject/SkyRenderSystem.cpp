#include "SkyRenderSystem.h"
#include "Camera.h"
#include "World.hpp"
#include "SkyDomeComponent.h"

void SkyRenderSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<SkyDomeComponent>())
	{
		auto skyDome = entity->GetComponent<SkyDomeComponent>();
		if (MainCamera != nullptr)
		{
			skyDome->UpdateCamera(MainCamera);
		}

		if ((skyDome != nullptr))
		{
			skyDome->Render();
		}
	}
}
