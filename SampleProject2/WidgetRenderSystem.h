#pragma once
#include "Define.h"
#include "System.hpp"

class WidgetRenderSystem : public ECS::System
{
private:
	ID3D11DeviceContext* Context;

public:
	WidgetRenderSystem();
	~WidgetRenderSystem();

public:
	virtual void Tick(ECS::World* world, float time) override;
};

