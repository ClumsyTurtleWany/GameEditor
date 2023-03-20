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
		public EventSubscriber<EFFECTEVENT_TIMEOUT>
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
		}

		virtual void release(World* world) override
		{
			world->unsubscribe<EFFECTEVENT_TIMEOUT>(this);
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
	};
}