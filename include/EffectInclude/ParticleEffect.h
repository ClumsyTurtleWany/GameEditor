#pragma once
#include "Entity.hpp"
#include "EditorECS\TransformComponent.h"
#include "EditorECS\MovementComponent.h"
#include "ParticleEffectComponent.h"

class ParticleEffect : public ECS::Entity
{
public:

public:
	ParticleEffect();
	ParticleEffect(std::wstring wszEffectName);
	ParticleEffect(std::wstring wszEffectName, TransformComponent tc);
	virtual ~ParticleEffect();
};

inline ParticleEffect::ParticleEffect()
{

}

inline ParticleEffect::ParticleEffect(std::wstring wszEffectName)
{
	this->AddComponent<TransformComponent>();
	this->AddComponent<MovementComponent>();
	this->AddComponent<ParticleEffectComponent>(wszEffectName);
}

inline ParticleEffect::ParticleEffect(std::wstring wszEffectName, TransformComponent TC)
{
	this->AddComponent<TransformComponent>(TC.Translation, TC.Rotation, TC.Scale);
	this->AddComponent<MovementComponent>();
	this->AddComponent<ParticleEffectComponent>(wszEffectName);
}

inline ParticleEffect::~ParticleEffect()
{
}
