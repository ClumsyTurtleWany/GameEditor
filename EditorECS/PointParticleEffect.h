#pragma once
#include "Entity.hpp"
#include "TransformComponent.h"
#include "MovementComponent.h"
#include "PointParticleSystemComponent.h"

class PointParticleEffect : public ECS::Entity
{
public:

public:
	PointParticleEffect();
	PointParticleEffect(TransformComponent tc);
	virtual ~PointParticleEffect();
};

PointParticleEffect::PointParticleEffect()
{
	this->AddComponent<TransformComponent>();
	this->AddComponent<MovementComponent>();
	this->AddComponent<PointParticleSystemComponent>();
}

inline PointParticleEffect::PointParticleEffect(TransformComponent TC)
{
	this->AddComponent<TransformComponent>(TC.Translation, TC.Rotation, TC.Scale);
	this->AddComponent<MovementComponent>();
	this->AddComponent<PointParticleSystemComponent>();
}

PointParticleEffect::~PointParticleEffect()
{
}
