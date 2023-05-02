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
	Vector3 Forward;
	Vector3 Destination;
	float	Speed;
	bool	IsMoving = false;			///< ���� �̵����̶�� true

	//2�� ������
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