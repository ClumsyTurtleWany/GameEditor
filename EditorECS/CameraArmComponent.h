#pragma once

class CameraArmComponent
{
public:
	float Distance = 0.0f;

	float Pitch = 0.0f;
	float Yaw = 0.0f;
	float Roll = 0.0f;
	
public:
	CameraArmComponent();
	virtual ~CameraArmComponent();
};