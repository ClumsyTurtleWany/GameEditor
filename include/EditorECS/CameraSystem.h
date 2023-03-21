#pragma once
#include "Define.h"
#include "System.hpp"

class CameraSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};