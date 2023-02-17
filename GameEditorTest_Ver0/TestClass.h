#pragma once
#include "GameCore.h"
#include "World.hpp"
#include "MousePicker.h"
#include "StaticMeshComponent.h"

class TestClass : public GameCore
{
public:
	ECS::World World;

public:
	MousePicker Picker;

	std::wstring SelectedFilename;

	std::map<std::wstring, std::unique_ptr<StaticMeshComponent>> StaticMeshMap;

public:
	TestClass(HWND hWnd);

public:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;

public:
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	DebugCamera* GetDebugCamera();
	bool CreateLandscape(int width, int height, int sectionSize);
};