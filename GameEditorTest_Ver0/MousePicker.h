#pragma once
#include "Define.h"
#include "Input.hpp"

class MousePicker
{
public:
	Ray MouseRay;
	float ClientWidth;
	float ClientHeight;

	Matrix View;
	Matrix Projection;

	Vector3 IntercetionPosition;

	bool isPick = false;

public:
	MousePicker();
	~MousePicker();

public:
	void Update();

public:
	bool CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2);
	bool IntersectTriangle(const Vector3& origin, const Vector3& direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float* distance);

};