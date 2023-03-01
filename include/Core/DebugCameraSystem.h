#pragma once
#include "Define.h"
#include "System.hpp"
#include "World.hpp"
#include "DebugCamera.h"
#include "Input.hpp"

class DebugCameraSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		DebugCamera* debugCamera = nullptr; //world->GetDebugCamera();
		for (auto& entity : world->GetEntities<DebugCamera>())
		{
			debugCamera = entity->GetComponent<DebugCamera>();
			break;
		}

		if (debugCamera != nullptr)
		{
			DirectX::XMFLOAT3 curPos;
			curPos.x = debugCamera->Pos.x;
			curPos.y = debugCamera->Pos.y;
			curPos.z = debugCamera->Pos.z;

			//////////////////////////// DebugCamera ////////////////////////////

			DirectX::XMMATRIX rotation;
			DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationRollPitchYaw(debugCamera->Pitch, debugCamera->Yaw, debugCamera->Roll);
			//DirectX::XMMATRIX world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, quaternion, DirectX::XMLoadFloat3(&curPos));
			DirectX::XMMATRIX world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, quaternion, debugCamera->Pos);
			DirectX::XMVECTOR determinant;
			DirectX::XMMATRIX view = DirectX::XMMatrixInverse(&determinant, world);

			//matView = *((Matrix4x4*)&view);
			debugCamera->View = view;
			debugCamera->Update();

			//debugCamera->Frustum = DirectX::BoundingFrustum(debugCamera->Projection);
			DirectX::BoundingFrustum newFrustum(debugCamera->Projection);
			newFrustum.Transform(debugCamera->Frustum, world);

			//frustum.createFrustum(&matView, &matProj_Perspective);
			//return true;
		}
	}

};