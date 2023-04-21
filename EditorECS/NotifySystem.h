#include "Define.h"
#include "System.hpp"
#include "AnimationComponent.h"
#include "EffectInclude/ParticleEffect.h"

/**
* @class NotifySystem
* @brief Notifier에 있는 정보를 토대로 이벤트 수신해서 실제 이펙트 발생시키는 시스템
*/
namespace ECS
{
	class NotifySystem : public System,
		public EventSubscriber<Notifier>
	{
	public:
		virtual ~NotifySystem() {}

		void init(World* world) override
		{
			world->subscribe<Notifier>(this);
		};

		void release(World* world) override
		{
			world->unsubscribe<Notifier>(this);
		}

		virtual void Tick(World* world, float deltaTime) override
		{
			//world->emit<TickEvent>({ Singleton<Timer>::GetInstance()->GetPlayTime() });
		}

		virtual void receive(class World* world, const Notifier& event) override
		{
			if (event.FModSound != nullptr)
			{
				event.FModSound->PlayEffect();
			}
			if (event.Sound != nullptr)
			{
				FMODSound* sound = FMODSoundManager::GetInstance()->GetSound(event.Sound->SoundName);
				if (sound != nullptr)
				{
					sound->PlayEffect();
					sound->SetVolume(event.Sound->Volume);
					sound->SetLoop(event.Sound->Loop);
				}
			}
			if (event.Effect != nullptr)
			{
				PlayEffect(world, event.Effect->Key, event.Effect->TransformComp, event.Effect->PSProp);
			}
		}

	};
}