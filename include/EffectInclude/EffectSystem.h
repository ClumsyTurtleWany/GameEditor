#include "EffectInclude\ParticleEffect.h"
#include "World.hpp"
#include "System.hpp"

namespace ECS
{
	struct EFFECTEVENT_TIMEOUT
	{
		Entity* entity;
	};

	class EffectSystem : public System,
		public EventSubscriber<EFFECTEVENT_TIMEOUT>,
		public EventSubscriber<EFFECTEVENT_MOVEUI_MODIFYCOLOR>,
		public EventSubscriber<EFFECTEVENT_SELECTUI_MODIFYCOLOR>
		//public EventSubscriber<CommonEvents::OnEntityDestroyed>,
		//public EventSubscriber<CommonEvents::OnComponentAdded<TransformComponent>>,
	{
	public:
		virtual void init(ECS::World* world) override
		{
			///////////////////////////////////////
			//EFFECTUTIL Initialization
			///////////////////////////////////////
			EFFECTUTIL::DXSTATE_MGR.init(DXDevice::g_pd3dDevice, DXDevice::g_pImmediateContext);
			EFFECTUTIL::FILEIO_MGR.init();
			EFFECTUTIL::SPRITE_MGR.init();
			EFFECTUTIL::EFFECT_MGR.init();

			world->subscribe<EFFECTEVENT_TIMEOUT>(this);
			world->subscribe<EFFECTEVENT_MOVEUI_MODIFYCOLOR>(this);
			world->subscribe<EFFECTEVENT_SELECTUI_MODIFYCOLOR>(this);
		}

		virtual void release(World* world) override
		{
			world->unsubscribe<EFFECTEVENT_TIMEOUT>(this);
			world->unsubscribe<EFFECTEVENT_MOVEUI_MODIFYCOLOR>(this);
			world->subscribe<EFFECTEVENT_SELECTUI_MODIFYCOLOR>(this);
		}

		virtual void Tick(World* world, float time) override
		{

		}

		virtual void receive(class World* world, const EFFECTEVENT_TIMEOUT& event) override
		{
			auto it = event.entity->GetComponent<ParticleEffectComponent>();
			if (it)
			{
				EFFECTUTIL::EFFECT_MGR.erase(it->m_pPPsystem);
				it->m_pPPsystem = nullptr;
				world->destroyEntity(event.entity, false);
			}
		}

		virtual void receive(class World* world, const EFFECTEVENT_MOVEUI_MODIFYCOLOR& event) override
		{
			auto it = event.entity->GetComponent<ParticleEffectComponent>();

			if (it)
			{
				it->m_pPPsystem->m_pEmitterList[0]->m_particles[0].initColor = event.CircleColor;
				it->m_pPPsystem->m_pEmitterList[0]->m_particles[0].lastColor = event.CircleColor;

				it->m_pPPsystem->m_pEmitterList[1]->m_particles[0].initColor = event.ArrowColor;
				it->m_pPPsystem->m_pEmitterList[1]->m_particles[0].lastColor = event.ArrowColor;
			}
		}

		virtual void receive(class World* world, const EFFECTEVENT_SELECTUI_MODIFYCOLOR& event) override
		{
			auto Circle = event.pCircle->GetComponent<ParticleEffectComponent>();
			auto Arrow = event.pArrow->GetComponent<ParticleEffectComponent>();

			if (Circle)
			{
				Circle->m_pPPsystem->m_pEmitterList[0]->m_particles[0].initColor = event.CircleColor;
				Circle->m_pPPsystem->m_pEmitterList[0]->m_particles[0].lastColor = event.CircleColor;
			}

			if (Arrow)
			{
				Arrow->m_pPPsystem->m_pEmitterList[0]->m_particles[0].initColor = event.ArrowColor;
				Arrow->m_pPPsystem->m_pEmitterList[0]->m_particles[0].lastColor = event.ArrowColor;
			}
		}
	};
}