#pragma once
#include "Define.h"
#include "BaseCore/Input.hpp"
#include "ECS/World.hpp"

#define MAX_PICK_DIST				10000.0f
#define PICKING_OP_TIME_LIMIT		0.0167f		//1 / 60 

//피킹 모드
enum PICKING_MODE
{
	PMOD_CHARACTER,
	PMOD_LAND,
	NUMBER_OF_PICKING_MODE
};

struct SelectState
{
	ECS::Entity*	pTarget;
	Vector3			vIntersection;
	float			fDistance;
};

class MousePicker
{
public:
	Input*			pMainInput;

	bool			bClicked;			
	DWORD			dwPickingButton;	//기본 : 왼쪽 마우스 버튼

	POINT			ptCursor;
	Ray				PickingRay;
	float			ClientWidth;
	float			ClientHeight;

	Matrix			World;
	Matrix			View;
	Matrix			Projection;
	Vector3			vIntersection;
	float			fIntersectionDistance;

	SelectState		lastSelect;			//이전 선택 상태
	SelectState		curSelect;			//현재 프레임에서의 선택 상태

	//피킹 모드 변수
	PICKING_MODE	optPickingMode;

	//피킹 작업 수행 딜레이를 위한 타이머와 입력 지연
	float			fLandTraceTimer;
	bool			bPendingClicked;
	bool			bOneTimeInputBlock;

public:
	MousePicker();
	~MousePicker();

public:
	void Update();
	void ClearSelectState();

public:
	void setMatrix(Matrix* pWorld, Matrix* pView, Matrix* pProj);

	//충돌 여부와 충돌 지점 둘 다 계산
	bool RayToOBB(const DirectX::BoundingOrientedBox& OBB);
	bool RayToAABB(const DirectX::BoundingBox& AABB);
	bool RayToSphere(const DirectX::BoundingSphere& sphere);

	bool CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2);
	bool IntersectTriangle(const Vector3& origin, const Vector3& direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float* distance);

	void SetPickingMode(PICKING_MODE pMode);

	static MousePicker& GetInstance()
	{
		static MousePicker singleInst;
		return singleInst;
	}
};

#define MAIN_PICKER MousePicker::GetInstance()