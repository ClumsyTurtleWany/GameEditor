#pragma once
#include "EditorCore.h"
#include "World.hpp"
#include "Camera.h"

#include "WindowsClient.h"
#include "Actor.h"
#include "PlaneComponent.h"
#include "CameraSystem.h"
#include "RenderSystem.h"

ECS_DEFINE_TYPE(SomeComponent);

#define swaping(a, b, t) ((t) = (a), (a) = (b), (b) = (t))

class SampleCore : public EditorCore
{
public:
	ECS::World MainWorld;
	Camera* MainCamera;
	Camera* SubCamera;
	Camera* temp;

	CameraSystem* MainCameraSystem = nullptr;
	RenderSystem* MainRenderSystem = nullptr;

	Actor* MainCameraActor = nullptr;
	Actor* SubCameraActor = nullptr;

	bool toggleCam;

public:
	SampleCore();
	~SampleCore();

public:
	virtual bool Initialize() override;
	virtual bool Frame() override;
	virtual bool Render() override;
	virtual bool Release() override;
};