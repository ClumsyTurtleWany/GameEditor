#pragma once
#include "EditorCore.h"
#include "World.hpp"
#include "Camera.h"

class SampleCore : public EditorCore
{
public:
	ECS::World MainWorld;
	Camera* DebugCamera;

public:
	SampleCore();
	~SampleCore();

public:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
};