#pragma once
#include "World.hpp"
#include "ParticleEmitter.h"

namespace EFFECTUTIL
{
	class EffectManager
	{
	private:
		std::map<std::wstring, PARTICLESYSTEM_FILE*>	m_PSFileMap;
		std::unordered_set<ParticleSystem*>				m_pPSSet;
		std::wstring									m_wszDefaultPointParticleFilePath;

		EffectManager();
		~EffectManager();

	public:
		EffectManager(const EffectManager& other) = delete;
		EffectManager& operator=(const EffectManager& other) = delete;

	public:
		bool init();
		bool release();

		bool loadDefaultEffectDir();
		bool loadPointParticleEffect(std::wstring wszEffectFilePath, std::wstring key);
		ParticleSystem* getPointParticleEffect(std::wstring key);

		bool createDefaultEmitter(ParticleEmitter** ppOut, POINTPARTICLEEMITTER_FILE& eFile);
		bool createTrailEmitter(ParticleEmitter** ppOut, POINTPARTICLEEMITTER_FILE& eFile);
		bool createBurstEmitter(ParticleEmitter** ppOut, POINTPARTICLEEMITTER_FILE& eFile);

		bool erase(ParticleSystem* pTarget);

		static EffectManager& getInstance()
		{
			static EffectManager singleInst;
			return singleInst;
		}
	};

	#define EFFECT_MGR EffectManager::getInstance()
}

