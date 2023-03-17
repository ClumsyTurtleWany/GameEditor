#include "Define.h"
#include "System.hpp"

/**
	* @class UpdateAnimSystem
	* @brief SkeletalMeshComponent의 BPAData를 업데이트하기 위한 시스템
	*/
class UpdateAnimSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};