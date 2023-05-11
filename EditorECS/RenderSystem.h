#pragma once
#include "Define.h"
#include "System.hpp"
#include "Camera.h"
#include "MotionTrail.h"

class RenderSystem : public ECS::System
{
public:
	Camera* MainCamera = nullptr;
	MotionTrail* pTest = nullptr;

public:
	virtual void Tick(ECS::World* world, float time) override;
};