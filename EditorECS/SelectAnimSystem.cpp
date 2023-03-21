#include "SelectAnimSystem.h"
#include "World.hpp"
#include "AnimationComponent.h"
#include "MovementComponent.h"


void SelectAnimSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<AnimationComponent, MovementComponent>())
	{
		auto animation = entity->GetComponent<AnimationComponent>();
		auto movement = entity->GetComponent<MovementComponent>();

		if (animation != nullptr && movement != nullptr)
		{
			if (movement->IsMoving && (animation->CurrentClipName == L"Idle"))
			{
				animation->SetClipByName(L"Run");
			}
			else if (movement->IsMoving == false&& (animation->CurrentClipName != L"Idle"))
			{
				animation->SetClipByName(L"Idle");
			}
			continue;
		}
	}
}