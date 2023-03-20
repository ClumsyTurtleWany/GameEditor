#pragma once
#include "ParticleEmitter.h"

namespace EFFECTUTIL
{
	struct POINTPARTICLEEMITTER_FILE
	{
		WCHAR							wszEmitterName[256];
		Vector3							vEmitterPos;
		PointParticleEmitterProperty	eProp;
		WCHAR							wszSpriteName[256];
		WCHAR							wszSpritePath[256];
		UINT							iSpriteType;
		UINT							spriteRC[2];
		WCHAR							wszTextureName[256];
		WCHAR							wszTexturePath[256];
		int								iBlendStateOption;
		bool							bEnableDepth;
		bool							bEnableDepthWrite;
	};

	struct PARTICLESYSTEM_FILE
	{
		WCHAR wszPSystemName[256];
		POINTPARTICLEEMITTER_FILE emitters[64];
	};

	struct ParticlesystemProperty
	{
		bool bLoop;

		float fDuration;
		float fStDelay;
		float fPlaySpeed;

		ParticlesystemProperty() 
		{
			bLoop = false;
			fDuration = 1.0f;
			fPlaySpeed = 1.0f;
			fStDelay = 0.0f;
		}
		ParticlesystemProperty(bool bLoop, float fDuration, float fStDelay, float fPlaySpeed)
		{
			this->bLoop = bLoop;
			this->fDuration = fDuration;
			this->fStDelay = fStDelay;
			this->fPlaySpeed = fPlaySpeed;
		}
	};

	class ParticleSystem
	{
	public:
		std::string											m_szName;
		std::wstring										m_wszName;

		ParticlesystemProperty								m_PSProp;
		float												m_fCurTime;
		bool												m_bPendingDelete;

		std::vector<EFFECTUTIL::ParticleEmitter*>			m_pEmitterList;

	public:
		ParticleSystem();
		~ParticleSystem();

		void setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);

		bool init();
		bool update();
		bool preRender(Matrix* pWorld, Matrix* pView, Matrix* pProj);
		bool render();
		bool release();

		void setProp(ParticlesystemProperty PSProp);
	};

	class EffectManager
	{
	private:
		std::map<std::wstring, PARTICLESYSTEM_FILE>		m_PSFileMap;
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

		bool erase(ParticleSystem* pTarget);

		static EffectManager& getInstance()
		{
			static EffectManager singleInst;
			return singleInst;
		}
	};

	#define EFFECT_MGR EffectManager::getInstance()
}

