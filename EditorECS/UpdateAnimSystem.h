#include "Define.h"
#include "System.hpp"

/**
	* @class UpdateAnimSystem
	* @brief SkeletalMeshComponent�� BPAData�� ������Ʈ�ϱ� ���� �ý���
	*/
class UpdateAnimSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};