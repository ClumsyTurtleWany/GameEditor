#pragma once
#include "Define.h"

/**
* @struct MovementComponent
* @brief �̵� ���� ������ ���� ������Ʈ
*/
class MovementComponent
{
public:
	Vector3 Location;
	Vector3 Forword;
	Vector3 Destination;
	float	Speed;
	bool	IsMoving = false;			///< ���� �̵����̶�� true

public:
	MovementComponent(); 
	MovementComponent(Vector3 location, Vector3 forword, Vector3 destination, float speed);
	virtual ~MovementComponent();
};