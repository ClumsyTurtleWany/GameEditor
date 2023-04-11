#pragma once
#include "Define.h"
#include "System.hpp"
#include "Camera.h"

class CameraSystem : public ECS::System
{
public:
	Camera* MainCamera = nullptr;
	Camera* TargetCamera = nullptr;

public:
	virtual void Tick(ECS::World* world, float time) override;
};