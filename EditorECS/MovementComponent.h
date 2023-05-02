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
	Vector3 Forward;
	Vector3 Destination;
	float	Speed;
	bool	IsMoving = false;			///< 현재 이동중이라면 true

	//2차 까지만
	Vector3 InitVelocity;
	Vector3 InitAcceleration;

	Vector3 CurrentVelocity;

public:
	MovementComponent(); 
	MovementComponent(Vector3 location, Vector3 forword, Vector3 destination, float speed);

	MovementComponent(Vector3 location, Vector3 destination, Vector3 initVelo, Vector3 initAccel);
	virtual ~MovementComponent();

public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);
};