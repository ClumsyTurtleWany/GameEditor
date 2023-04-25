#pragma once
#include "Entity.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "MovementComponent.h"

class Actor : public ECS::Entity
{
public:
	TransformComponent* Transform;

public:
	Actor();
	virtual ~Actor();

public:
	virtual bool Save(std::string filename) override;
	virtual bool Load(std::string filename) override;
};