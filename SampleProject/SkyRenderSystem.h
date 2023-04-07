#pragma once
#include "System.hpp"
#include "Camera.h"

class SkyRenderSystem : public ECS::System
{
public:
	Camera* MainCamera = nullptr;

public:
	virtual void Tick(ECS::World* world, float time) override;
};

