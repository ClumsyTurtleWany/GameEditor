#pragma once
#include "ECSCommon.hpp"

namespace ECS
{
	class Component
	{
	private:
		int id;

	public:
		Component() {};
		virtual ~Component() {};

	public:
		virtual bool Save(std::wstring filename) { return true; };
		virtual bool Load(std::wstring filename) { return true; };
	};

	template<typename T>
	class ComponentContainer : public Component
	{
	public:
		T Data;

	public:
		ComponentContainer() {};
		ComponentContainer(const T& data) : Data(data) {};
		virtual ~ComponentContainer() {};
	};
}