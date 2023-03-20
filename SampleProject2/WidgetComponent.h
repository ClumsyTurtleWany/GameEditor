#pragma once
#include "TransformComponent.h"
// #include "MeshComponent.hpp"
// #include "CameraMatrix.h" ¹Ø¿¡ "WidgetObject.h" ·Î °¬À½
#include "WidgetObject.h"

class WidgetComponent
{
public:
	WidgetComponent();
	~WidgetComponent();

public:
	void UpdateTransformMatrix(const TransformComponent& transform);

public:
	bool Initialize();
	bool Render();

public:
	WidgetObject* FindObj(std::wstring ObjName);
	void AddObj(std::wstring ObjName, WidgetObject* wo);

private:
	ID3D11DeviceContext* Context;
	TransformMatrix	TransformData;

public:
	std::vector<WidgetObject*> ObjList;
	std::map<std::wstring, WidgetObject*> ObjMap;
};

