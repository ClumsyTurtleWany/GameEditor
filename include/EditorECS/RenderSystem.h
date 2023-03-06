#pragma once
#include "Define.h"
#include "System.hpp"

class RenderSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};