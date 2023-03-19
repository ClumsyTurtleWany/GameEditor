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

	class ParticleSystem
	{
		friend class EffectManager;
	private:
		std::string											m_szName;
		std::wstring										m_wszName;

		std::vector<EFFECTUTIL::ParticleEmitter*>		m_pEmitterList;

	public:
		ParticleSystem();
		~ParticleSystem();

		void setDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pDContext);

		bool init();
		bool update();
		bool preRender(Matrix* pWorld, Matrix* pView, Matrix* pProj);
		bool render();
		bool release();
	};

	class EffectManager
	{
	private:
		std::map<std::wstring, ParticleSystem*>	m_PPSMap;
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

		static EffectManager& getInstance()
		{
			static EffectManager singleInst;
			return singleInst;
		}
	};

	#define EFFECT_MGR EffectManager::getInstance()
}

