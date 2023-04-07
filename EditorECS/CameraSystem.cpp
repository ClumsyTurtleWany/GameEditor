#include "CameraSystem.h"
#include "World.hpp"
#include "Camera.h"
#include "TransformComponent.h"
#include "CameraArmComponent.h"
#include "SplineComponent.h"

void CameraSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<Camera, TransformComponent>())
	{
		auto camera = entity->GetComponent<Camera>();
		auto transform = entity->GetComponent<TransformComponent>();
		auto cameraArm = entity->GetComponent<CameraArmComponent>();
		auto spline = entity->GetComponent<SplineComponent>();
		
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
			else
			{
				float pitch = camera->Pitch;
				float roll = camera->Roll;
				float yaw = camera->Yaw;

				if (cameraArm != nullptr)
				{
					pitch = cameraArm->Pitch + transform->Rotation.y;
					roll = cameraArm->Roll + transform->Rotation.x;
					yaw = cameraArm->Yaw + transform->Rotation.z;

					Vector3 dist = Vector3(0.0f, 0.0f, -cameraArm->Distance);
					Vector3 rotation = Vector3(roll, pitch, yaw) / 180.0f * PI;
					DirectX::FXMVECTOR q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
					DirectX::XMMATRIX world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, q, transform->Translation);
					Vector3 vecDist = DirectX::XMVector3Transform(dist, world);

					camera->Pos.x = vecDist.x;
					camera->Pos.y = vecDist.y;
					camera->Pos.z = vecDist.z;
				}

				Vector3 rotation = Vector3(roll, pitch, yaw) / 180.0f * PI;
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