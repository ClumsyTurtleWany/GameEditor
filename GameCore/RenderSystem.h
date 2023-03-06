#pragma once
#include "Define.h"
#include "System.hpp"

class RenderSystem : public ECS::System
{
private:
	ID3D11DeviceContext* Context;

public:
	RenderSystem();
	~RenderSystem();

public:
	virtual void Tick(ECS::World* world, float time) override;
	void SetContext(ID3D11DeviceContext* context);
};