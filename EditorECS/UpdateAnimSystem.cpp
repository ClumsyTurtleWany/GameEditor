#include "UpdateAnimSystem.h"
#include "World.hpp"
#include "AnimationComponent.h"
#include "SkeletalMeshComponent.h"

void UpdateAnimSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<SkeletalMeshComponent, AnimationComponent>())
	{
		auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
		auto animation = entity->GetComponent<AnimationComponent>();

		if ((skeletalMesh != nullptr) && (animation != nullptr))
		{
			animation->UpdateAnim(skeletalMesh, time);
		}
	}
}