#include "CameraSystem.h"
#include "World.hpp"
#include "Camera.h"
#include "TransformComponent.h"
#include "CameraArmComponent.h"
#include "SplineComponent.h"
#include "MovementComponent.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"

void CameraSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<Camera, TransformComponent>())
	{
		auto camera = entity->GetComponent<Camera>();
		auto transform = entity->GetComponent<TransformComponent>();
		auto cameraArm = entity->GetComponent<CameraArmComponent>();
		auto spline = entity->GetComponent<SplineComponent>();
		auto movement = entity->GetComponent<MovementComponent>();
		auto box = entity->GetComponent<BoundingBoxComponent>();
		
		if (camera != nullptr)
		{
			if (spline)
			{
				camera->Pos = spline->m_curKey.vPos;

				Matrix world = DirectX::XMMatrixAffineTransformation(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(), spline->m_curKey.qRot, camera->Pos);
				Matrix invView = Matrix::CreateFromQuaternion(spline->m_curKey.qRot);
				invView *= Matrix::CreateTranslation(camera->Pos);

				camera->View = invView.Invert();
				camera->Update();
				DirectX::BoundingFrustum newFrustum(camera->Projection);
				newFrustum.Transform(camera->Frustum, world);
			}
			else if (movement && box)
			{
				if (movement->IsMoving)
				{
					// ¹æÇâº¤ÅÍ
					Vector3 Up = -Vector3::Up;
					Vector3 Right = Up.Cross(movement->Forward);
					Right.Normalize();

					camera->Pos = box->OBB.Center + Right * 100.0f;

					camera->View = DirectX::XMMatrixLookAtLH(camera->Pos, (Vector3)box->OBB.Center, Vector3::Up);
					camera->Update();

					Matrix invView;
					camera->View.Invert(invView);

					DirectX::BoundingFrustum newFrustum(camera->Projection);
					newFrustum.Transform(camera->Frustum, invView);
				}
			}
			else
			{
				float pitch = camera->Pitch;
				float yaw = camera->Yaw;
				float roll = camera->Roll;

				if (cameraArm != nullptr)
				{
					pitch = cameraArm->Pitch + transform->Rotation.x;
					yaw = cameraArm->Yaw + transform->Rotation.y;
					roll = cameraArm->Roll + transform->Rotation.z;

					Vector3 dist = Vector3(0.0f, 0.0f, -cameraArm->Distance);
					Vector3 rotation = Vector3(pitch, yaw, roll) / 180.0f * PI;
					DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
					DirectX::XMMATRIX world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, q, transform->Translation);
					Vector3 vecDist = DirectX::XMVector3Transform(dist, world);
				}

				Vector3 rotation = Vector3(pitch, yaw, roll) / 180.0f * PI;
				DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
				DirectX::XMMATRIX world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, q, camera->Pos);
				DirectX::XMVECTOR determinant;
				DirectX::XMMATRIX view = DirectX::XMMatrixInverse(&determinant, world);

				camera->View = view;
				camera->Update();

				DirectX::BoundingFrustum newFrustum(camera->Projection);
				newFrustum.Transform(camera->Frustum, world);
			}
		}
	}

	if (MainCamera != nullptr)
	{
		MainCamera->Apply();
	}
}