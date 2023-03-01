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
	Entities.push_back(newEntity);
}

std::vector<std::shared_ptr<ECS::Entity>>& ECS::World::GetAllEntities()
{
	return Entities;
}

ECS::System* ECS::World::AddSystem(ECS::System* system)
{
	std::shared_ptr<System> newSystem(system);
	Systems.push_back(newSystem);
	return system;
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
