#include "Define.h"
#include "System.hpp"


	/**
	* @class SelectAnimSystem
	* @brief AnimationComponent�� AnimationClip�� ��ü�ϱ� ���� �ý���
	*/
class SelectAnimSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override;
};