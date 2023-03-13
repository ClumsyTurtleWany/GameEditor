#pragma once
#include "ECSCommon.hpp"

namespace ECS
{
	class System
	{
	public:
		/**
		* @brief	월드에 시스템이 추가 될 때 호출되는 함수
		* @details	이벤트 구독기를 시스템에 바인딩하고자 하는 경우 이 함수를 재정의한다.
		*/
		virtual void init(World* world) {}

		/**
		* @brief	월드에서 시스템이 제거 될 때 호출되는 함수 
		* @details	이벤트 구독기를 시스템에 바인딩한 경우 이 함수의 재정의를 통해 해제한다.
		*/
		virtual void release(World* world) {}

		virtual void Tick(World* world, float time) = 0;
	};
}