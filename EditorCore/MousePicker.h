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

	Matrix  World;
	Matrix	View;
	Matrix	Projection;
	Vector3 Intersection;

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
	void setMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj);

	//충돌 여부와 충돌 지점 둘 다 계산
	bool RayToOBB(const DirectX::BoundingOrientedBox& OBB);
	bool RayToSphere(const DirectX::BoundingSphere& sphere);

	bool CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2);
	bool IntersectTriangle(const Vector3& origin, const Vector3& direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float* distance);

	static MousePicker& GetInstance()
	{
		static MousePicker singleInst;
		return singleInst;
	}
};

#define MAIN_PICKER MousePicker::GetInstance()