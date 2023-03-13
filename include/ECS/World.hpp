#pragma once
#include "ECSCommon.hpp"
#include "System.hpp"
#include "Entity.hpp"
#include "EventSubscriber.hpp"

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

		std::unordered_map<EventID, 
			std::vector<ECS::BaseEventSubscriber*>> subscribers;

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

		void destroyEntity(Entity* entity, bool immediate = false);

		template <typename T>
		std::vector<ECS::Entity*> GetEntities();

		template <typename T, typename U, typename... Types>
		std::vector<ECS::Entity*> GetEntities();

		template <typename T>
		const int GetCount();

		template <typename T, typename U, typename... Types>
		const int GetCount();

		void CleanUp();

		//---------------------------------------------
		// System
		//---------------------------------------------
		System* AddSystem(System* system);
		void RemoveSystem(System* system);

		void EnableSystem(System* system);
		void DisableSystem(System* system);


		//---------------------------------------------
		// Event Handling
		//---------------------------------------------
		template<typename EventType>
		void subscribe(EventSubscriber<EventType>* subscriber);

		template<typename EventType>
		void unsubscribe(EventSubscriber<EventType>* subscriber);

		void unsubscribeAll(void* subscriber);

		template<typename EventType>
		void emit(const EventType& event);
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

	/**
	* @brief	�̺�Ʈ �����⸦ ����Ѵ�.
	* @details	���� EventID�� ������ ��� ���� �̺�Ʈ �����⸦ ���ο� ������� ��ü�Ѵ�.
	*/
	template<typename EventType>
	inline void World::subscribe(EventSubscriber<EventType>* subscriber)
	{
		EventID id = ECS::GetEventID<EventType>();
		auto found = subscribers.find(id);
		if (found == subscribers.end())
		{
			std::vector<ECS::BaseEventSubscriber*> subList;
			subList.push_back(subscriber);
			subscribers.insert(std::make_pair(id, subList));
		}
		else
		{
			found->second.push_back(subscriber);
		}
	}

	/**
	* @brief	�μ��� �ѱ� �̺�Ʈ ������� ���� ID�� ��ϵ� �̺�Ʈ �����⸦ �����Ѵ�.
	*/
	template<typename EventType>
	inline void World::unsubscribe(EventSubscriber<EventType>* subscriber)
	{
		EventID id = ECS::GetEventID<EventType>();
		auto found = subscribers.find(id);
		if (found != subscribers.end())
		{
			for (size_t idx = 0; idx < found->second.size(); )
			{
				auto* foundSub = found->second[idx];
				if (foundSub == subscriber)
				{
					found->second.erase(found->second.begin() + idx);
				}
				else
				{
					idx++;
				}
			}
		}
	}

	/**
	* @brief	�̺�Ʈ�� ���忡 �����Ѵ�.
	* @details	Ÿ�Կ� �´� �̺�Ʈ �����Ⱑ ������ �ƹ� �۾��� ���� �ʴ´�.
	*/
	template<typename EventType>
	inline void World::emit(const EventType& event)
	{
		EventID id = ECS::GetEventID<EventType>();
		auto found = subscribers.find(id);
		if (found != subscribers.end())
		{
			for (ECS::BaseEventSubscriber* base : found->second)
			{
				auto* sub = reinterpret_cast<EventSubscriber<EventType>*>(base);
				sub->receive(this, event);
			}
		}
	}
}