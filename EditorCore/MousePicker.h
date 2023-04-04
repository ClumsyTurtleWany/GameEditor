#pragma once
#include "Define.h"
#include "BaseCore/Input.hpp"
#include "ECS/World.hpp"

#define MAX_PICK_DIST				10000.0f
#define PICKING_OP_TIME_LIMIT		0.0167f		//1 / 60 

//��ŷ ���
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
	DWORD			dwPickingButton;	//�⺻ : ���� ���콺 ��ư

	POINT			ptCursor;
	Ray				PickingRay;
	float			ClientWidth;
	float			ClientHeight;

	Matrix			World;
	Matrix			View;
	Matrix			Projection;
	Vector3			vIntersection;
	float			fIntersectionDistance;

	SelectState		lastSelect;			//���� ���� ����
	SelectState		curSelect;			//���� �����ӿ����� ���� ����

	//��ŷ ��� ����
	PICKING_MODE	optPickingMode;

	//��ŷ �۾� ���� �����̸� ���� Ÿ�̸ӿ� �Է� ����
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

	//�浹 ���ο� �浹 ���� �� �� ���
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