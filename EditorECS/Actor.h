#pragma once
#include "Entity.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "MovementComponent.h"

class Actor : public ECS::Entity
{
public:

public:
	Actor();
	virtual ~Actor();
};