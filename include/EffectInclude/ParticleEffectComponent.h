#pragma once
#include "EffectManager.h"

struct ParticleEffectComponent
{
	EFFECTUTIL::ParticleSystem*							m_pPPsystem;

	ParticleEffectComponent();
	ParticleEffectComponent(std::wstring wszEffectName);
	ParticleEffectComponent(std::wstring wszEffectName, 
								EFFECTUTIL::ParticlesystemProperty PSProp);
	~ParticleEffectComponent();
};

inline ParticleEffectComponent::ParticleEffectComponent()
{
	m_pPPsystem = nullptr;
}

inline ParticleEffectComponent::ParticleEffectComponent(std::wstring wszEffectName)
{
	m_pPPsystem = EFFECTUTIL::EFFECT_MGR.getPointParticleEffect(wszEffectName);
}

inline ParticleEffectComponent::ParticleEffectComponent(std::wstring wszEffectName, EFFECTUTIL::ParticlesystemProperty PSProp)
{
	m_pPPsystem = EFFECTUTIL::EFFECT_MGR.getPointParticleEffect(wszEffectName);

	if (m_pPPsystem)
	{
		m_pPPsystem->setProp(PSProp);
	}
}

inline ParticleEffectComponent::~ParticleEffectComponent()
{

}