#include "MovementSystem.h"
#include "MovementComponent.h"
#include "TransformComponent.h"

void MovementSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<TransformComponent, MovementComponent>())
	{
		auto transform = entity->GetComponent<TransformComponent>();
		auto movement = entity->GetComponent<MovementComponent>();
		
		movement->Location = transform->Translation;
		Vector3 diff = movement->Destination - movement->Location;
		// ADD -> 목적지에 도달했다면 IsMoving = false
		if (diff.Distance(movement->Destination, movement->Location) <= 0.5f)
		{
			movement->Location = movement->Destination;
			transform->Translation = movement->Location;
			movement->IsMoving = false;
			continue;
		}			
		else 
			movement->IsMoving = true;

		movement->Forword = diff;
		movement->Forword.Normalize();

		Vector3 basisAxis = Vector3(0.0f, 0.0f, -1.0f);
		float pitch = acos(basisAxis.Dot(movement->Forword) / (basisAxis.LengthSquared() * movement->Forword.LengthSquared())) / PI * 180.0f;
		if (movement->Forword.x < 0.0f)
		{			
			transform->Rotation.y = pitch;
		}
		else
		{
			transform->Rotation.y = -pitch;
		}

		movement->Location += movement->Forword * movement->Speed * time;
		transform->Translation = movement->Location;
	}
}