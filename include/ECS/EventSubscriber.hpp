#pragma once
#include "ECSCommon.hpp"

namespace ECS
{
	/**
	* @brief 임의의 이벤트 구독기를 STL컨테이너에 보관하기 위한 기반 클래스
	*/
	class BaseEventSubscriber
	{
	public:
		virtual ~BaseEventSubscriber() {};
	};

	/**
	* @brief 이벤트 구독 클래스
	*/
	template<typename EventType>
	class EventSubscriber : public BaseEventSubscriber
	{
	public:
		virtual ~EventSubscriber() {}

		/**
		* @brief 월드에서 이벤트가 발생했을 때 호출되는 함수
		*/
		virtual void receive(World* world, const EventType& event) = 0;
	};

	namespace CommonEvents
	{
		/**
		* @brief Entity 생성시
		*/
		struct OnEntityCreated
		{
			Entity* entity;
		};

		/**
		* @brief Entity 소멸시
		*/
		struct OnEntityDestroyed
		{
			Entity* entity;
		};

		/**
		* @brief Entity에 컴포넌트 추가시
		*/
		template<typename ComponentType>
		struct OnComponentAdded
		{
			Entity* entity;
			ComponentType* pComponent;
		};

		/**
		* @brief Entity에 컴포넌트 제거시
		*/
		template<typename ComponentType>
		struct OnComponentRemoved
		{
			Entity* entity;
			ComponentType* pComponent;
		};
	}
}