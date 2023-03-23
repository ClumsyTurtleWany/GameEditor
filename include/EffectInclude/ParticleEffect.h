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

inline void PlayEffect(ECS::World* pWorld, 
	std::wstring key,
	Vector3 pos, 
	EFFECTUTIL::ParticlesystemProperty PSProp = { false, 1.0f, 0.0f, 1.0f })
{
	if (pWorld)
	{
		ParticleEffect* newE = new ParticleEffect(key, TransformComponent(pos, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }), PSProp);

		pWorld->AddEntity(newE);
	}
}

inline void PlayEffect(ECS::World* pWorld,
	std::wstring key,
	TransformComponent tc,
	EFFECTUTIL::ParticlesystemProperty PSProp = { false, 1.0f, 0.0f, 1.0f })
{
	if (pWorld)
	{
		ParticleEffect* newE = new ParticleEffect(key, tc, PSProp);

		pWorld->AddEntity(newE);
	}
}