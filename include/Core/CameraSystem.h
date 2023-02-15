#pragma once
#include "Define.h"
#include "System.hpp"
#include "World.hpp"
#include "DebugCamera.h"


class CameraSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		for (auto& entity : world->GetEntities<Camera>())
		{
			auto comp = entity.get()->GetComponent<Camera>();
			world->SetMainCamera(comp);
			break;
		}


	}
};