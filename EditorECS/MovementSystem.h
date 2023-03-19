#pragma once
#include "System.hpp"
#include "World.hpp"

class MovementSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};