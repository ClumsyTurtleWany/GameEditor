#pragma once
#include "Entity.hpp"
#include "TransformComponent.h"
#include "MovementComponent.h"
#include "ParticleEffectComponent.h"

class ParticleEffect : public ECS::Entity
{
public:
	ParticleEffect();
	ParticleEffect(std::wstring wszEffectName);
	ParticleEffect(std::wstring wszEffectName, TransformComponent tc);
	ParticleEffect(std::wstring wszEffectName, EFFECTUTIL::ParticlesystemProperty PSProp);
	ParticleEffect(std::wstring wszEffectName,
		TransformComponent tc,
		EFFECTUTIL::ParticlesystemProperty PSProp);
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

inline ParticleEffect::ParticleEffect(std::wstring wszEffectName, TransformComponent tc)
{
	this->AddComponent<TransformComponent>(tc.Translation, tc.Rotation, tc.Scale);
	this->AddComponent<MovementComponent>();
	this->AddComponent<ParticleEffectComponent>(wszEffectName);
}

inline ParticleEffect::ParticleEffect(std::wstring wszEffectName, EFFECTUTIL::ParticlesystemProperty PSProp)
{
	this->AddComponent<TransformComponent>();
	this->AddComponent<MovementComponent>();
	this->AddComponent<ParticleEffectComponent>(wszEffectName, PSProp);
}

inline ParticleEffect::ParticleEffect(std::wstring wszEffectName, TransformComponent tc, EFFECTUTIL::ParticlesystemProperty PSProp)
{
	this->AddComponent<TransformComponent>(tc.Translation, tc.Rotation, tc.Scale);
	this->AddComponent<MovementComponent>();
	this->AddComponent<ParticleEffectComponent>(wszEffectName, PSProp);
}

inline ParticleEffect::~ParticleEffect()
{
}
