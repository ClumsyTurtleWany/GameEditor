#include "MovementSystem.h"
#include "MovementComponent.h"
#include "TransformComponent.h"
#include "SplineComponent.h"
#include "OscillationComponent.h"
#include "Camera.h"

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

		movement->Forward = diff;
		movement->Forward.Normalize();

		Vector3 basisAxis = Vector3::Forward;
		Vector3 PYR = DirectX::XMVector3AngleBetweenVectors(basisAxis, movement->Forward);
		float yaw = (PYR.y / PI) * 180.0f;

		if (movement->Forward.x < 0.0f)
		{			
			transform->Rotation.y = yaw;
		}
		else
		{
			transform->Rotation.y = -yaw;
		}

		movement->Location += movement->Forward * movement->Speed * time;
		transform->Translation = movement->Location;
	}

	for (auto& entity : world->GetEntities<SplineComponent>())
	{
		auto spline = entity->GetComponent<SplineComponent>();

		if (spline && !spline->m_bPaused)
		{
			spline->update(time);
		}
	}

	for (auto& entity : world->GetEntities<OscillationComponent>())
	{
		auto oscillation = entity->GetComponent<OscillationComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		auto camera = entity->GetComponent<Camera>();

		if (camera != nullptr)
		{
			oscillation->Update(camera->Pos, time);
		}
		else
		{
			if (transform != nullptr)
			{
				oscillation->Update(transform->Translation, time);
			}
		}		
	}
}