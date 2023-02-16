#pragma once
#include "Define.h"
#include "Input.hpp"

class MousePicker
{
public:
	Ray MouseRay;
	float ClientWidth;
	float ClientHeight;

	Matrix Projection;
	Matrix View;

public:
	MousePicker();
	~MousePicker();

public:
	void Update();

public:
	bool CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2);


};