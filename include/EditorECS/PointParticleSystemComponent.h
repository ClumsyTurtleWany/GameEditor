#pragma once
#include "PointParticleEmitter.h"

struct PointParticleSystemComponent
{
	Matrix viewMat;
	Matrix projMat;

	std::vector<EFFECTUTIL::PointParticleEmitter*>				m_pEmitterList;

	PointParticleSystemComponent();
	~PointParticleSystemComponent();
};

PointParticleSystemComponent::PointParticleSystemComponent()
{

}

PointParticleSystemComponent::~PointParticleSystemComponent()
{

}