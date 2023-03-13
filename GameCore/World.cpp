#include "World.hpp"

void ECS::World::Tick(float time)
{
	CleanUp();
	for (auto& system : Systems)
	{
		system.get()->Tick(this, time);
	}
}

void ECS::World::AddEntity(ECS::Entity* entity)
{
	std::shared_ptr<Entity> newEntity(entity);
	newEntity->pWorld = this;
	Entities.push_back(newEntity);

	emit<ECS::CommonEvents::OnEntityCreated>({ newEntity.get() });
}

std::vector<std::shared_ptr<ECS::Entity>>& ECS::World::GetAllEntities()
{
	return Entities;
}

/**
* @brief	Entity를 제거한다.
* @details	Entity제거 이벤트를 반드시 발생시킨다.
*/
void ECS::World::destroyEntity(Entity* entity, bool immediate)
{
	if (entity == nullptr)
		return;

	if (entity->IsDestroy)
	{
		if (immediate)
		{
			for (size_t idx = 0; idx < Entities.size(); )
			{
				auto* foundSub = Entities[idx].get();
				if (foundSub == entity)
				{
					Entities.erase(Entities.begin() + idx);
				}
				else
				{
					idx++;
				}
			}
		}

		return;
	}

	entity->IsDestroy = true;

	emit<CommonEvents::OnEntityDestroyed>({ entity });

	if (immediate)
	{
		for (size_t idx = 0; idx < Entities.size(); )
		{
			auto* foundSub = Entities[idx].get();
			if (foundSub == entity)
			{
				Entities.erase(Entities.begin() + idx);
			}
			else
			{
				idx++;
			}
		}
	}
}

ECS::System* ECS::World::AddSystem(ECS::System* system)
{
	std::shared_ptr<System> newSystem(system);
	newSystem->init(this);
	Systems.push_back(newSystem);
	return system;
}

void ECS::World::RemoveSystem(System* system)
{
	system->release(this);

	for (size_t idx = 0; idx < Systems.size(); )
	{
		auto* foundSub = Systems[idx].get();
		if (foundSub == system)
		{
			Systems.erase(Systems.begin() + idx);
		}
		else
		{
			idx++;
		}
	}
}

void ECS::World::EnableSystem(ECS::System* system)
{
	std::shared_ptr<System> enableSystem(system);
	auto it = std::find(DisableSystems.begin(), DisableSystems.end(), enableSystem);
	if (it != DisableSystems.end())
	{
		DisableSystems.erase(it);
		Systems.push_back(enableSystem);
	}
}

void ECS::World::DisableSystem(ECS::System* system)
{
	std::shared_ptr<System> disableSystem(system);
	auto it = std::find(Systems.begin(), Systems.end(), disableSystem);
	if (it != Systems.end())
	{
		Systems.erase(it);
		DisableSystems.push_back(disableSystem);
	}
}

/**
* @brief	모든 이벤트 구독기를 월드에서 제거한다.
*/
void ECS::World::unsubscribeAll(void* subscriber)
{
	for (auto it : subscribers)
	{
		for (size_t idx = 0; idx < it.second.size(); )
		{
			auto* foundSub = it.second[idx];
			if (foundSub == subscriber)
			{
				it.second.erase(it.second.begin() + idx);
			}
			else
			{
				idx++;
			}
		}
	}
}

void ECS::World::CleanUp()
{
	for (size_t idx = 0; idx < Entities.size(); )
	{
		auto entity = Entities[idx].get();
		if (entity->IsDestroy)
		{
			Entities.erase(Entities.begin() + idx);
		}
		else
		{
			idx++;
		}
	}
}
