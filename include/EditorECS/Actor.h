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

Actor::Actor()
{
	this->Name = L"Actor" + std::to_wstring(this->ID);
	this->AddComponent<TransformComponent>();
}

Actor::~Actor()
{
	
}