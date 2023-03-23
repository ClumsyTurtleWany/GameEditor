#pragma once
#include "Define.h"
#include "System.hpp"
#include "Camera.h"

class RenderSystem : public ECS::System
{
public:
	Camera* MainCamera = nullptr;

public:
	virtual void Tick(ECS::World* world, float time) override;
};