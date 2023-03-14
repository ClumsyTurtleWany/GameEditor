#pragma once
#include "System.hpp"

class SkyRenderSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};

