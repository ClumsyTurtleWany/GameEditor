#pragma once
#include "Actor.h"
#include "MovementComponent.h"
#include "ArrowComponent.h"

class Character : public Actor
{
public:
	MovementComponent*		MovementComp;
	ArrowComponent*			Arrow;

public:
	Character();
	virtual ~Character();
	
public:
	bool MoveTo(Vector3 destination);
};