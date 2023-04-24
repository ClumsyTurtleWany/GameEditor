#include "UpdateAnimSystem.h"
#include "World.hpp"
#include "AnimationComponent.h"
#include "SkeletalMeshComponent.h"
#include "SocketComponent.h"
#include "StaticMeshComponent.h"


void UpdateAnimSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<SkeletalMeshComponent, AnimationComponent>())
	{
		auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
		auto animation = entity->GetComponent<AnimationComponent>();
		auto transform = entity->GetComponent<TransformComponent>();

		if ((skeletalMesh != nullptr) && (animation != nullptr))
		{
			animation->UpdateAnim(skeletalMesh, time);
			animation->Notify(world);
		}
		if (entity->has<SocketComponent>())
		{
			auto socket = entity->GetComponent<SocketComponent>();
			//socket->Update(*animation,*transform);
			socket->Update(*skeletalMesh,*transform);
		}
	}
}