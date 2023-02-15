#pragma once
#include "Camera.h"

class DebugCamera : public Camera
{
public:
	float Speed = 10.0f;
	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float Roll = 0.0f;

public:
	DebugCamera() {};
	virtual ~DebugCamera() {};
};