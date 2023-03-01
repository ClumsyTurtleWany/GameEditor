#pragma once
#include "EditorCore.h"
#include "World.hpp"
#include "MousePicker.h"
#include "StaticMeshComponent.h"
#include "Camera.h"

class MainCore : public EditorCore
{
public:
	ECS::World EditorWorld;
	Camera* DebugCamera;

public:
	MousePicker Picker;

	std::wstring SelectedFilename;

	std::map<std::wstring, std::unique_ptr<StaticMeshComponent>> StaticMeshMap;

public:
	MainCore();
	virtual ~MainCore();

public:
	bool Resize(int x, int y, int width, int height);
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	Camera* GetCamera();
	bool CreateLandscape(int width, int height, int sectionSize);
	bool AddSelectedEntity();
	bool Sculpting();
};