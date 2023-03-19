#pragma once
#include "Actor.h"
#include "MovementComponent.h"

class Character : public Actor
{
public:
	MovementComponent* MovementComp;

public:
	Character();
	virtual ~Character();
	
public:
	bool MoveTo(Vector3 destination);
};