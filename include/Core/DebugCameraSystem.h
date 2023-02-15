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
		DebugCamera* debugCamera = world->GetDebugCamera();
		if (debugCamera != nullptr)
		{
			KeyState keyState_LButton = Input::getInstance()->getKey(VK_LBUTTON);
			if (keyState_LButton == KeyState::Hold)
			{
				POINT ptOffset = Input::getInstance()->m_ptOffset;
				debugCamera->Yaw += ptOffset.x * time;
				debugCamera->Pitch += ptOffset.y * time;
			}

			KeyState keyState_Space = Input::getInstance()->getKey(VK_SPACE);
			if (keyState_Space == KeyState::Hold)
			{
				debugCamera->Speed = 100.0f;
			}
			else
			{
				debugCamera->Speed = 50.0f;
			}

			KeyState KeyState_W = Input::getInstance()->getKey('W');
			if (KeyState_W == KeyState::Hold)
			{
				debugCamera->Pos += debugCamera->Look * debugCamera->Speed * time;
			}

			KeyState KeyState_S = Input::getInstance()->getKey('S');
			if (KeyState_S == KeyState::Hold)
			{
				debugCamera->Pos -= debugCamera->Look * debugCamera->Speed * time;
			}

			KeyState KeyState_A = Input::getInstance()->getKey('A');
			if (KeyState_A == KeyState::Hold)
			{
				debugCamera->Pos -= debugCamera->Right * debugCamera->Speed * time;
			}

			KeyState KeyState_D = Input::getInstance()->getKey('D');
			if (KeyState_D == KeyState::Hold)
			{
				debugCamera->Pos += debugCamera->Right * debugCamera->Speed * time;
			}

			KeyState KeyState_Q = Input::getInstance()->getKey('Q');
			if (KeyState_Q == KeyState::Hold)
			{
				debugCamera->Pos += debugCamera->Up * debugCamera->Speed * time;
			}

			KeyState KeyState_E = Input::getInstance()->getKey('E');
			if (KeyState_E == KeyState::Hold)
			{
				debugCamera->Pos -= debugCamera->Up * debugCamera->Speed * time;
			}

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
			debugCamera->Frustum = DirectX::BoundingFrustum(debugCamera->Projection);

			//frustum.createFrustum(&matView, &matProj_Perspective);
			//return true;
		}
	}

};