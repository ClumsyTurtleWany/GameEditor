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
		Vector3 forword = movement->Destination - movement->Location;
		// ADD -> 목적지에 도달했다면 IsMoving = false
		if (forword.Distance(movement->Destination, movement->Location) <= 0.01f)
		{
			movement->IsMoving = false;
			continue;
		}			
		else 
			movement->IsMoving = true;

		movement->Forword = forword;
		movement->Forword.Normalize();

		// (0.0f, 0.0f, -1.0f) : roll = 0.0f, pitch = 0.0f, yaw = 0.0f
		// (-1.0f, 0.0f, 0.0f) : roll = 0.0f, pitch = 90.0f, yaw = 0.0f
		// (0.0f, 0.0f, 1.0f) : roll = 0.0f, pitch = 180.0f, yaw = 0.0f
		// (1.0f, 0.0f, 0.0f) : roll = 0.0f, pitch = 270.0f or -90.0f, yaw = 0.0f

		movement->Location += movement->Forword * movement->Speed * time;
		transform->Translation = movement->Location;
	}
}