#pragma once
#include "Define.h"
#include "System.hpp"
#include "World.hpp"
#include "TransformComponent.h"

class TransformSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		for (auto& entity : world->GetEntities<TransformComponent>())
		{

		}
	}
};