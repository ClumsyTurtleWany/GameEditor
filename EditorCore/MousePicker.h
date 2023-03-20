//#pragma once
//#include "Define.h"
//#include "Input.hpp"
//#include "ECS/World.hpp"
//
//class MousePicker
//{
//public:
//	Vector2	ptCursor;
//	Ray		PickingRay;
//	float	ClientWidth;
//	float	ClientHeight;
//
//	Matrix	View;
//	Matrix	Projection;
//
//	Vector3 IntercetionPosition;
//
//	ECS::Entity* pTarget;
//
//	bool isPickBVolume = false;
//	bool isPickUI = false;
//
//public:
//	MousePicker();
//	~MousePicker();
//
//public:
//	void Update();
//
//public:
//	bool CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2);
//	bool IntersectTriangle(const Vector3& origin, const Vector3& direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float* distance);
};