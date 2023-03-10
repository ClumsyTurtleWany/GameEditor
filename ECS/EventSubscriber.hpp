#pragma once
#include "ECSCommon.hpp"

namespace ECS
{
	/**
	* @brief ������ �̺�Ʈ �����⸦ STL�����̳ʿ� �����ϱ� ���� ��� Ŭ����
	*/
	class BaseEventSubscriber
	{
	public:
		virtual ~BaseEventSubscriber() {};
	};

	/**
	* @brief �̺�Ʈ ���� Ŭ����
	*/
	template<typename EventType>
	class EventSubscriber : public BaseEventSubscriber
	{
	public:
		virtual ~EventSubscriber() {}

		/**
		* @brief ���忡�� �̺�Ʈ�� �߻����� �� ȣ��Ǵ� �Լ�
		*/
		virtual void receive(World* world, const EventType& event) = 0;
	};

	namespace CommonEvents
	{
		/**
		* @brief Entity ������
		*/
		struct OnEntityCreated
		{
			Entity* entity;
		};

		/**
		* @brief Entity �Ҹ��
		*/
		struct OnEntityDestroyed
		{
			Entity* entity;
		};

		/**
		* @brief Entity�� ������Ʈ �߰���
		*/
		template<typename ComponentType>
		struct OnComponentAdded
		{
			Entity* entity;
			ComponentType* pComponent;
		};

		/**
		* @brief Entity�� ������Ʈ ���Ž�
		*/
		template<typename ComponentType>
		struct OnComponentRemoved
		{
			Entity* entity;
			ComponentType* pComponent;
		};
	}
}