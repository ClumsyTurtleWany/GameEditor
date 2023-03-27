#pragma once
#include "Define.h"
#include "BaseCore/Input.hpp"
#include "ECS/World.hpp"

//피킹 모드
enum PICKING_MODE
{
	PMOD_DEFAULT,
	PMOD_MOVE,
	PMOD_SELECTTARGET,
	PMOD_ATTACKTARGET,
	NUMBER_OF_PICKING_MODE
};

class MousePicker
{
public:
	Input*	pMainInput;

	POINT	ptCursor;
	Ray		PickingRay;
	float	ClientWidth;
	float	ClientHeight;

	Matrix  World;
	Matrix	View;
	Matrix	Projection;
	Vector3 Intersection;

	ECS::Entity* pTarget;

	//피킹 모드 변수
	PICKING_MODE optPickingMode;

public:
	MousePicker();
	~MousePicker();

public:
	void Update();

public:
	void setMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj);

	//충돌 여부와 충돌 지점 둘 다 계산
	bool RayToOBB(const DirectX::BoundingOrientedBox& OBB);
	bool RayToAABB(const DirectX::BoundingBox& AABB);
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