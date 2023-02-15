#include "World.hpp"

void ECS::World::Tick(float time)
{
	for (auto& system : Systems)
	{
		system.get()->Tick(this, time);
	}
}

void ECS::World::SetMainCamera(Camera* camera)
{
	/*if (MainCamera == nullptr)
	{
		MainCamera = std::make_shared<Camera>(camera);
	}
	else*/
	{
		MainCamera.reset(camera);
	}
}

void ECS::World::SetDebugCamera(DebugCamera* camera)
{
	/*if (DebuggingCamera == nullptr)
	{
		DebuggingCamera = std::make_shared<DebugCamera>(camera);
	}
	else*/
	{
		DebuggingCamera.reset(camera);
	}
}

Camera* ECS::World::GetMainCamera()
{
	return MainCamera.get();
}

DebugCamera* ECS::World::GetDebugCamera()
{
	return DebuggingCamera.get();
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