#include "SkyRenderSystem.h"
#include "Camera.h"
#include "World.hpp"
#include "SkyDomeComponent.h"

void SkyRenderSystem::Tick(ECS::World* world, float time)
{
	Camera* mainCamera = nullptr;
	for (auto& ent : world->GetEntities<Camera>())
	{
		mainCamera = ent->GetComponent<Camera>();
	}

	for (auto& entity : world->GetEntities<SkyDomeComponent>())
	{
		auto skyDome = entity->GetComponent<SkyDomeComponent>();
		if (mainCamera != nullptr)
		{
			skyDome->UpdateCamera(mainCamera);
		}

		if ((skyDome != nullptr))
		{
			skyDome->Render();
		}
	}
}
