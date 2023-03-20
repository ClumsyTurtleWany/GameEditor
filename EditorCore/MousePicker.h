#pragma once
#include "Define.h"
#include "BaseCore/Input.hpp"
#include "ECS/World.hpp"

class MousePicker
{
public:
	POINT	ptCursor;
	Ray		PickingRay;
	float	ClientWidth;
	float	ClientHeight;

	Matrix	View;
	Matrix	Projection;
	Vector3 IntercetionPosition;

	ECS::Entity* pTarget;

	bool isPickBVolume;
	bool isPickUI;

public:
	MousePicker();
	~MousePicker();

public:
	void Update();
	void clearState();

public:
	bool CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2);
	bool IntersectTriangle(const Vector3& origin, const Vector3& direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float* distance);

	static MousePicker& GetInstance()
	{
		static MousePicker singleInst;
		return singleInst;
	}
};

#define MAIN_PICKER MousePicker::GetInstance()