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
	ParticleEffect(std::wstring wszEffectName, 
		Vector3 stPos, Vector3 edPos, 
		Vector3 velocity, Vector3 accel, 
		Vector3 scale, EFFECTUTIL::ParticlesystemProperty PSProp);
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

inline ParticleEffect::ParticleEffect(std::wstring wszEffectName, Vector3 stPos, Vector3 edPos, Vector3 velocity, Vector3 accel, Vector3 scale, EFFECTUTIL::ParticlesystemProperty PSProp)
{
	auto transform = this->AddComponent<TransformComponent>();
	transform->Translation = stPos;
	transform->Scale = scale;

	auto movement = this->AddComponent<MovementComponent>();
	movement->Location = stPos;
	movement->Destination = edPos;

	movement->InitVelocity = velocity;
	movement->InitAcceleration = accel;

	movement->CurrentVelocity = movement->InitVelocity;

	this->AddComponent<ParticleEffectComponent>(wszEffectName, PSProp);
}

inline ParticleEffect::~ParticleEffect()
{
}

inline void PlayEffect(ECS::World* pWorld, 
	std::wstring effectName,
	Vector3 pos, 
	EFFECTUTIL::ParticlesystemProperty PSProp = { false, 1.0f, 0.0f, 1.0f })
{
	if (pWorld)
	{
		ParticleEffect* newE = new ParticleEffect(effectName, TransformComponent(pos, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }), PSProp);

		pWorld->AddEntity(newE);
	}
}

inline void PlayEffect(ECS::World* pWorld,
	std::wstring effectName,
	TransformComponent tc,
	EFFECTUTIL::ParticlesystemProperty PSProp = { false, 1.0f, 0.0f, 1.0f })
{
	if (pWorld)
	{
		ParticleEffect* newE = new ParticleEffect(effectName, tc, PSProp);

		pWorld->AddEntity(newE);
	}
}

inline void PlaySimpleTrail(ECS::World* pWorld,
	std::wstring effectName,
	Vector3 stPos, Vector3 edPos, 
	float fVelocity, float fAccel,
	Vector3 scale, EFFECTUTIL::ParticlesystemProperty PSProp = { false, 1.0f, 0.0f, 1.0f })
{
	if (pWorld)
	{
		Vector3 Direction = edPos - stPos;
		Direction.Normalize();

		ParticleEffect* newE = new ParticleEffect(effectName, stPos, edPos, Direction * fVelocity, Direction * fAccel, scale, PSProp);

		Matrix world = Matrix::CreateScale(scale) * Matrix::CreateTranslation(stPos);
		newE->GetComponent<ParticleEffectComponent>()->m_pPPsystem->setMatrix(&world, nullptr, nullptr);

		pWorld->AddEntity(newE);
	}
}