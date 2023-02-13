#pragma once
#include "ECSCommon.hpp"

namespace ECS
{
	class System
	{
	public:
		virtual void Tick(World* world, float time) = 0;
	};
}