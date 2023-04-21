#pragma once
#include "Camera.h"

class ActionCameraComponent : public Camera
{
public:
	Vector3 PrevLook;
	Vector3 NextLook;
	Vector3 PrevPos;
	float InterpolationTime;
	float ElapseTime = 0.0f;
	bool isLockOn = false;

public:
	ActionCameraComponent();
	virtual ~ActionCameraComponent();
};