#pragma once
#include "System.hpp"
#include "World.hpp"


/**
* @class TimelineSystem
* @brief Ÿ�Ӷ����� �ð��� ������Ʈ�ϴ� �ý���
*/
class TimelineSystem : public ECS::System
{
public:
	void Tick(ECS::World* world, float time) override;
};