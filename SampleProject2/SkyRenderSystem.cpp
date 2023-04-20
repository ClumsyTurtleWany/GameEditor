#include "SkyRenderSystem.h"
#include "Camera.h"
#include "World.hpp"
#include "SkyDomeComponent.h"
#include "CloudDomeComponent.h"

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

	for (auto& entity : world->GetEntities<CloudDomeComponent>())
	{
		auto cloudDome = entity->GetComponent<CloudDomeComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		if (transform != nullptr)
		{
			transform->Scale = Vector3(4000.0f, 4000.0f, 4000.0f);
			transform->Rotation.y += cloudDome->RotationSpeed * time;
			cloudDome->UpdateTransformMatrix(*transform);
			cloudDome->UpdateCamera(MainCamera);
			cloudDome->Render();
		}
	}
}
