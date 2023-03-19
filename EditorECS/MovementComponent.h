#pragma once
#include "Define.h"

class MovementComponent
{
public:
	Vector3 Location;
	Vector3 Forword;
	Vector3 Destination;
	float	Speed;

public:
	MovementComponent(); 
	MovementComponent(Vector3 location, Vector3 forword, Vector3 destination, float speed);
	virtual ~MovementComponent();
};