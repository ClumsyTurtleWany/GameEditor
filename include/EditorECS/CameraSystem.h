#pragma once
#include "Define.h"
#include "System.hpp"
#include "Camera.h"

class CameraSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		for (auto& entity : world->GetEntities<Camera>())
		{
			auto camera = entity->GetComponent<Camera>();
			if (camera != nullptr)
			{
				DirectX::XMFLOAT3 curPos;
				curPos.x = camera->Pos.x;
				curPos.y = camera->Pos.y;
				curPos.z = camera->Pos.z;

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

};