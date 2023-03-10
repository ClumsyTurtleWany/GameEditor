#pragma once
#include "ECSCommon.hpp"

namespace ECS
{
	class System
	{
	public:
		/**
		* @brief	���忡 �ý����� �߰� �� �� ȣ��Ǵ� �Լ�
		* @details	�̺�Ʈ �����⸦ �ý��ۿ� ���ε��ϰ��� �ϴ� ��� �� �Լ��� �������Ѵ�.
		*/
		virtual void init(World* world) {}

		/**
		* @brief	���忡�� �ý����� ���� �� �� ȣ��Ǵ� �Լ� 
		* @details	�̺�Ʈ �����⸦ �ý��ۿ� ���ε��� ��� �� �Լ��� �����Ǹ� ���� �����Ѵ�.
		*/
		virtual void release(World* world) {}

		virtual void Tick(World* world, float time) = 0;
	};
}