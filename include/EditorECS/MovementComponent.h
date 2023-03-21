#pragma once
#include "Define.h"

/**
* @struct MovementComponent
* @brief 이동 관련 정보를 담은 컴포넌트
*/
class MovementComponent
{
public:
	Vector3 Location;
	Vector3 Forword;
	Vector3 Destination;
	float	Speed;
	bool	IsMoving = false;			///< 현재 이동중이라면 true

public:
	MovementComponent(); 
	MovementComponent(Vector3 location, Vector3 forword, Vector3 destination, float speed);
	virtual ~MovementComponent();
};