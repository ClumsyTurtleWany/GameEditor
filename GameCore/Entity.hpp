#pragma once
#include "ECSCommon.hpp"
#include "Component.hpp"
#include "EventSubscriber.hpp"

static int EntityID = 0;

class ECS::World;

namespace ECS
{
	class Entity
	{
	public:
		friend class World;

	protected:
		std::unordered_map<ComponentID, std::shared_ptr<ECS::Component>> Components;
		int ID;
		std::wstring Name;

		World* pWorld;

	public:
		bool IsDestroy = false;

	public:
		Entity() 
		{ 
			ID = EntityID++; 
			pWorld = nullptr;
		};
		virtual ~Entity() {};

	public:
		template<typename ComponentType, typename... ComponentArgs>
		ComponentType* AddComponent(ComponentArgs&&... args)
		{
			ComponentID id = ECS::GetComponentID<ComponentType>();
			auto it = Components.find(id);
			if (it != Components.end())
			{
				ComponentContainer<ComponentType>* container = reinterpret_cast<ComponentContainer<ComponentType>*>(it->second.get());
				container->Data = ComponentType(args...);

				//auto temp = { this, &(container->Data) };
				//pWorld->emit<ECS::CommonEvents::OnComponentAdded>(temp);
				
				return &container->Data;
			}
			else
			{
				ComponentContainer<ComponentType>* container = new ComponentContainer<ComponentType>();
				container->Data = ComponentType(args...);
				std::shared_ptr<ComponentContainer<ComponentType>> newComp(container);
				Components.insert(std::make_pair(id, newComp));

				return &container->Data;
			}
			
		}

		template<typename ComponentType>
		ComponentType* GetComponent()
		{
			ComponentID id = ECS::GetComponentID<ComponentType>();
			auto it = Components.find(id);
			if (it == Components.end())
			{
				return nullptr;
			}

			ComponentContainer<ComponentType>* container = reinterpret_cast<ComponentContainer<ComponentType>*>(it->second.get());
			return &container->Data;
		}

		template<typename ComponentType>
		bool RemoveComponent()
		{
			ComponentID id = ECS::GetComponentID<ComponentType>();
			auto it = Components.find(id);
			if (it != Components.end())
			{
				//auto temp = { this, &it.second };
				//pWorld->emit<ECS::CommonEvents::OnComponentRemoved>(temp);

				Components.erase(it);
				return true;
			}

			return false;
		}

		template <typename T>
		bool has() const
		{
			ComponentID id = ECS::GetComponentID<T>();
			auto it = Components.find(id);
			if (it != Components.end())
			{
				return true;
			}
			
			return false;
		}

		template <typename T, typename U, typename... Types>
		bool has() const
		{
			return has<T>() && has<U, Types...>();
		}

		int GetID() const
		{
			return ID;
		}

		std::wstring GetName() const
		{
			return Name;
		}
	};

}