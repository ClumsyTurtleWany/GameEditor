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
				// 방향벡터
				Vector3 Up = Vector3::Up;
				Vector3 BaseRight = Vector3::Right;
				Vector3 InvRight = -Up.Cross(movement->Forward);
				InvRight.Normalize();

				Vector3 dest = box->OBB.Center - InvRight * (box->OBB.Extents.x + box->OBB.Extents.z) * 0.5f * 10.0f;

				Quaternion cur = Quaternion::CreateFromAxisAngle(Up, BaseRight.Dot(camera->Right));
				Quaternion rot = Quaternion::CreateFromAxisAngle(Up, BaseRight.Dot(InvRight));
				Quaternion res;

				//1초동안 해당거리를 추적하도록 보간 => 가까이 붙을 수록 느려짐
				if (Vector3::DistanceSquared(camera->Pos, dest) < 0.25f)
				{
					camera->Pos = dest;
					res = rot;
				}
				else
				{
					camera->Pos = Vector3::Lerp(camera->Pos, dest, min(time * 3.0f, 1.0f));
					res = Quaternion::Slerp(cur, rot, min(time * 3.0f, 1.0f));
				}

				Matrix world = DirectX::XMMatrixAffineTransformation(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(), res, camera->Pos);
				Matrix invView = Matrix::CreateFromQuaternion(res);
				invView *= Matrix::CreateTranslation(camera->Pos);

				camera->View = invView.Invert();
				camera->Update();
				DirectX::BoundingFrustum newFrustum(camera->Projection);
				newFrustum.Transform(camera->Frustum, world);
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