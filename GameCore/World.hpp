#pragma once
#include "ECSCommon.hpp"
#include "System.hpp"
#include "Entity.hpp"
#include "DebugCamera.h"

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

	private:
		std::shared_ptr<DebugCamera>	DebuggingCamera;
		std::shared_ptr<Camera>			MainCamera;

	public:
		//---------------------------------------------
		// Common
		//---------------------------------------------
		void Tick(float time);
		void			SetMainCamera(Camera* camera);
		void			SetDebugCamera(DebugCamera* camera);

		Camera*			GetMainCamera();
		DebugCamera*	GetDebugCamera();

		//---------------------------------------------
		// Entity
		//---------------------------------------------
		void AddEntity(Entity* entity);
		std::vector<std::shared_ptr<ECS::Entity>>& GetAllEntities();

		template <typename T>
		std::vector<std::shared_ptr<ECS::Entity>> GetEntities();

		template <typename T, typename U, typename... Types>
		std::vector<std::shared_ptr<ECS::Entity>> GetEntities();

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
	std::vector<std::shared_ptr<ECS::Entity>> World::GetEntities()
	{
		std::vector<std::shared_ptr<ECS::Entity>> entities;
		for (auto& ent : Entities)
		{
			if (ent.get()->has<T>())
			{
				entities.push_back(ent);
			}
		}

		return entities;
	}

	template <typename T, typename U, typename... Types>
	std::vector<std::shared_ptr<ECS::Entity>> World::GetEntities()
	{
		std::vector<std::shared_ptr<ECS::Entity>> entities;
		for (auto& ent : Entities)
		{
			if (ent.get()->has<T, U, Types...>())
			{
				entities.push_back(ent);
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