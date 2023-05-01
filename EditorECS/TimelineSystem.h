#pragma once
#include "System.hpp"
#include "World.hpp"


/**
* @class TimelineSystem
* @brief 타임라인의 시간을 업데이트하는 시스템
*/
class TimelineSystem : public ECS::System
{
public:
	void Tick(ECS::World* world, float time) override;
};