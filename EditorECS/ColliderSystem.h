#pragma once
#include "Define.h"
#include "System.hpp"

class ColliderSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};