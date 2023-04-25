#include "CameraSystem.h"
#include "World.hpp"
#include "Camera.h"
#include "TransformComponent.h"
#include "CameraArmComponent.h"
#include "SplineComponent.h"
#include "MovementComponent.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"
#include "ActionCameraComponent.h"
#include "OscillationComponent.h"

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
			else if (camera == MainCamera && TargetCamera)
			{
				camera->Pos = Vector3::Lerp(camera->Pos, TargetCamera->Pos, min(time, 1.0f));
				camera->lastQ = Quaternion::Slerp(camera->lastQ, TargetCamera->lastQ, min(time, 1.0f));
				camera->Target = TargetCamera->Target;

				Matrix newView = Matrix::Lerp(MainCamera->View, TargetCamera->View, min(time, 1.0f));

				if (0)
				{
					Matrix matWorld = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, camera->lastQ, camera->Pos);

					DirectX::XMVECTOR determinant;
					DirectX::XMMATRIX view = DirectX::XMMatrixInverse(&determinant, matWorld);

					camera->View = view;
					camera->Update();

					DirectX::BoundingFrustum newFrustum(camera->Projection);
					newFrustum.Transform(camera->Frustum, matWorld);
				}
				else
				{
					camera->CreateViewMatrix(camera->Pos, TargetCamera->Target, Vector3(0.0f, 1.0f, 0.0f));
					//camera->View = newView; // 팀장님 추가본, 근데 지금 차원축이 좀 뒤틀려서 잠시 주석,,
					camera->Update();

					DirectX::XMVECTOR determinant;
					DirectX::XMMATRIX matWorld = DirectX::XMMatrixInverse(&determinant, camera->View);

					DirectX::BoundingFrustum newFrustum(camera->Projection);
					newFrustum.Transform(camera->Frustum, matWorld);
				}
			}
			else
			{
				float pitch = camera->Pitch;
				float yaw = camera->Yaw;
				float roll = camera->Roll;

				Quaternion q;
				Matrix world;

				if (cameraArm != nullptr)
				{
					pitch = cameraArm->Pitch + transform->Rotation.x;
					yaw = cameraArm->Yaw + transform->Rotation.y;
					roll = cameraArm->Roll + transform->Rotation.z;

					Vector3 dist = Vector3::Forward * cameraArm->Distance;
					if (box) dist.y += box->OBB.Center.y;

					Vector3 rotation = Vector3(pitch, yaw, roll) / 180.0f * PI;
					q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
					world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, q, transform->Translation);
					Vector3 vecDist = DirectX::XMVector3Transform(dist, world);

					camera->Pos.x = vecDist.x;
					camera->Pos.y = vecDist.y;
					camera->Pos.z = vecDist.z;
				}

				Vector3 rotation = Vector3(pitch, yaw, roll) / 180.0f * PI;
				q = DirectX::XMQuaternionRotationRollPitchYawFromVector(rotation);
				world = DirectX::XMMatrixAffineTransformation({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, q, camera->Pos);

				camera->lastQ = q;

				DirectX::XMVECTOR determinant;
				DirectX::XMMATRIX view = DirectX::XMMatrixInverse(&determinant, world);
				camera->View = view;
				camera->Update();

				DirectX::BoundingFrustum newFrustum(camera->Projection);
				newFrustum.Transform(camera->Frustum, world);
			}
		}
	}

	for (auto& entity : world->GetEntities<ActionCameraComponent, TransformComponent>())
	{
		auto camera = entity->GetComponent<ActionCameraComponent>();
		auto spline = entity->GetComponent<SplineComponent>();
		auto oscillation = entity->GetComponent<OscillationComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		if (transform != nullptr)
		{
			camera->Pos = transform->Translation;
		}

		if (spline)
		{
			if (!spline->m_CPList.empty())
			{
				camera->Pos = spline->m_curKey.vPos;
			}
		}

		if (camera->isLockOn)
		{
			if (camera->ElapseTime < camera->InterpolationTime)
			{
				camera->ElapseTime += time;
				Vector3 lerp = DirectX::XMVectorLerp(camera->PrevLook, camera->NextLook, camera->ElapseTime / camera->InterpolationTime);
				camera->Look = lerp;
			}
			else
			{
				camera->Look = camera->NextLook;
				camera->isLockOn = false;
			}
			camera->CreateViewMatrix(camera->Pos, camera->Look, Vector3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			Vector3 diff = camera->Pos - camera->PrevPos;
			camera->Look = camera->Target + diff;
		}

		camera->CreateViewMatrix(camera->Pos, camera->Look, Vector3(0.0f, 1.0f, 0.0f));

		camera->Update();

		Matrix world = camera->View.Invert();

		DirectX::BoundingFrustum newFrustum(camera->Projection);
		newFrustum.Transform(camera->Frustum, world);

		camera->PrevPos = camera->Pos;
	}

	if (MainCamera != nullptr)
	{
		MainCamera->Apply();
	}
}