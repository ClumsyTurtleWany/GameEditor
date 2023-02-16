#pragma once
#include "GameCore.h"
#include "World.hpp"
#include "MousePicker.h"

class TestClass : public GameCore
{
private:
	ECS::World World;

public:
	MousePicker Picker;

public:
	TestClass(HWND hWnd);

public:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
};