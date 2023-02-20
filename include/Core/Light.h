#pragma once
#include "Entity.hpp"
#include "MovementComponent.h"
#include "TransformComponent.h"
#include "LightComponent.h"

class Light : public ECS::Entity
{
public:


public:
	Light();
	virtual ~Light();
};

Light::Light()
{
	this->AddComponent<MovementComponent>();
	this->AddComponent<TransformComponent>();
}

Light::~Light()
{

}