#pragma once
#include "EffectManager.h"

struct ParticleEffectComponent
{
	EFFECTUTIL::ParticleSystem* pPPsystem;

	ParticleEffectComponent();
	ParticleEffectComponent(std::wstring wszEffectName);
	~ParticleEffectComponent();
};

inline ParticleEffectComponent::ParticleEffectComponent()
{
	pPPsystem = nullptr;
}

inline ParticleEffectComponent::ParticleEffectComponent(std::wstring wszEffectName)
{
	pPPsystem = EFFECTUTIL::EFFECT_MGR.getPointParticleEffect(wszEffectName);
}

inline ParticleEffectComponent::~ParticleEffectComponent()
{

}