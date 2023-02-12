#pragma once
#include "System.hpp"
#include "World.hpp"
#include "MovementComponent.h"

class MovementSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		for (auto& entity : world->GetAllEntities())
		{
			auto it = entity.get()->GetComponent<MovementComponent>();
			if (!it)
			{
				continue;
			}

			it->Location = it->Location + (it->Forword * it->Speed) * time;
		}
	}
};