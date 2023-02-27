#pragma once
#include "ECSCommon.hpp"
#include "System.hpp"
#include "Entity.hpp"

namespace ECS
{
	class World
	{
	public:
		friend class System;
		friend class Entity;

	private:
		std::vector<std::shared_ptr<ECS::Entity>> Entities;
		std::vector<std::shared_ptr<ECS::System>> Systems;
		std::vector<std::shared_ptr<ECS::System>> DisableSystems;

	public:
		//---------------------------------------------
		// Common
		//---------------------------------------------
		void			Tick(float time);

		//---------------------------------------------
		// Entity
		//---------------------------------------------
		void AddEntity(Entity* entity);
		std::vector<std::shared_ptr<ECS::Entity>>& GetAllEntities();

		template <typename T>
		std::vector<ECS::Entity*> GetEntities();

		template <typename T, typename U, typename... Types>
		std::vector<ECS::Entity*> GetEntities();

		template <typename T>
		const int GetCount();

		template <typename T, typename U, typename... Types>
		const int GetCount();

		//---------------------------------------------
		// System
		//---------------------------------------------
		System* AddSystem(System* system);
		void EnableSystem(System* system);
		void DisableSystem(System* system);
	};

	template <typename T>
	std::vector<ECS::Entity*> World::GetEntities()
	{
		std::vector<ECS::Entity*> entities;
		for (auto& ent : Entities)
		{
			if (ent.get()->has<T>())
			{
				entities.push_back(ent.get());
			}
		}

		return entities;
	}

	template <typename T, typename U, typename... Types>
	std::vector<ECS::Entity*> World::GetEntities()
	{
		std::vector<ECS::Entity*> entities;
		for (auto& ent : Entities)
		{
			if (ent.get()->has<T, U, Types...>())
			{
				entities.push_back(ent.get());
			}
		}

		return entities;
	}

	template<typename T>
	inline const int World::GetCount()
	{
		int cnt = 0;
		for (auto& ent : Entities)
		{
			if (ent.get()->has<T>())
			{
				cnt++;
			}
		}
		return cnt;
	}

	template <typename T, typename U, typename... Types>
	inline const int World::GetCount()
	{
		int cnt = 0;
		for (auto& ent : Entities)
		{
			if (ent.get()->has<T, U, Types...>())
			{
				cnt++;
			}
		}
		return cnt;
	}
}