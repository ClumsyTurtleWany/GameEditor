#include "MovementSystem.h"
#include "MovementComponent.h"
#include "TransformComponent.h"
#include "ArrowComponent.h"
#include "SplineComponent.h"
#include "OscillationComponent.h"
#include "Camera.h"

void MovementSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<TransformComponent, MovementComponent>())
	{
		auto transform = entity->GetComponent<TransformComponent>();
		auto movement = entity->GetComponent<MovementComponent>();
		auto arrow = entity->GetComponent<ArrowComponent>();
		
		if (transform && movement && arrow)
		{
			movement->Location = transform->Translation;
			movement->Forward = arrow->Forward;

			Vector3 diff = movement->Destination - movement->Location;
			diff.Normalize();
			float dir = diff.Dot(arrow->Forward);

			// ADD -> 목적지에 도달했다면 IsMoving = false
			if (diff.Distance(movement->Destination, movement->Location) <= 0.5f || dir < 0)
			{
				movement->Location = movement->Destination;
				transform->Translation = movement->Location;
				movement->IsMoving = false;
				continue;
			}			
			else 
				movement->IsMoving = true;

			transform->Rotation.y = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenNormals(Vector3::Forward, arrow->Forward)) / PI * 180.0f;
			if (movement->Forward.x > 0.0f) { transform->Rotation.y = -transform->Rotation.y; }

			movement->Location += movement->Forward * movement->Speed * time;
			transform->Translation = movement->Location;
		}
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
		auto movement = entity->GetComponent<MovementComponent>();

		if (camera != nullptr)
		{
			oscillation->Update(camera->Pos, time);
		}
		else
		{
			if (transform != nullptr && movement != nullptr)
			{
				oscillation->Update(transform->Translation, time);
			}
		}		
	}
}