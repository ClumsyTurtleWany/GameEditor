#include "CameraSystem.h"
#include "World.hpp"
#include "Camera.h"
#include "TransformComponent.h"

void CameraSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<Camera, TransformComponent>())
	{
		auto camera = entity->GetComponent<Camera>();
		auto transform = entity->GetComponent<TransformComponent>();
		if (camera != nullptr)
		{
			//camera->Pos.x = transform->Translation.x;
			//camera->Pos.y = transform->Translation.y;
			//camera->Pos.z = transform->Translation.z - 100.0f;

			//////////////////////////// DebugCamera ////////////////////////////

			DirectX::XMMATRIX rotation;
			DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationRollPitchYaw(camera->Pitch, camera->Yaw, camera->Roll);
			DirectX::XMMATRIX world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, quaternion, camera->Pos);
			DirectX::XMVECTOR determinant;
			DirectX::XMMATRIX view = DirectX::XMMatrixInverse(&determinant, world);

			camera->View = view;
			camera->Update();

			DirectX::BoundingFrustum newFrustum(camera->Projection);
			newFrustum.Transform(camera->Frustum, world);
		}
	}

}