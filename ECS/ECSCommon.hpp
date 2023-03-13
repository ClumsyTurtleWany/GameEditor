#pragma once
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <functional>
#include <bitset>
#include <array>

namespace ECS
{
	#define ECS_DEFINE_TYPE(name)

	class World;
	class Entity;
	class Component;
	class System;

	class BaseEventSubscriber;

	using ComponentID = std::type_index;
	using EventID = std::type_index;

	template <typename T>
	ComponentID GetComponentID()
	{
		return ComponentID(typeid(T));
	}

	template <typename T>
	EventID GetEventID()
	{
		return EventID(typeid(T));
	}
}