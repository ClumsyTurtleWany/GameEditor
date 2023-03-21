#include "Define.h"
#include "System.hpp"


	/**
	* @class SelectAnimSystem
	* @brief AnimationComponent의 AnimationClip을 교체하기 위한 시스템
	*/
class SelectAnimSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};