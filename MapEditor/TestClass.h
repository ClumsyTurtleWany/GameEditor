#pragma once
#include "GameCore.h"
#include "World.hpp"

class TestClass : public GameCore
{
private:
	ECS::World World;

public:
	TestClass(HWND hWnd);

public:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
};