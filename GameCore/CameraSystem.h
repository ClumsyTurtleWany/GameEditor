#pragma once
#include "Define.h"
#include "System.hpp"
#include "World.hpp"
#include "Camera.h"


class CameraSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		for (auto& entity : world->GetEntities<Camera>())
		{
			auto comp = entity.get()->GetComponent<Camera>();


		}
	}

	void CreateViewMatrix(Camera& camera, const Vector3& eye, const Vector3& target, const Vector3& up)
	{
		camera.Pos = eye;
		camera.Look = target;
		camera.Up = up;

		Vector3 direction = target;
		direction -= eye;
		direction.Normalize();
		Vector3 right = up.Cross(direction);
		right.Normalize();
		Vector3 newUp = direction.Cross(right);
		newUp.Normalize();

		camera.View._11 = right.x; camera.View._12 = newUp.x; camera.View._13 = direction.x;
		camera.View._21 = right.y; camera.View._22 = newUp.y; camera.View._23 = direction.y;
		camera.View._31 = right.z; camera.View._32 = newUp.z; camera.View._33 = direction.z;
		camera.View._41 = -(camera.Pos.x * camera.View._11 + camera.Pos.y * camera.View._21 + camera.Pos.z * camera.View._31);
		camera.View._42 = -(camera.Pos.x * camera.View._12 + camera.Pos.y * camera.View._22 + camera.Pos.z * camera.View._32);
		camera.View._43 = -(camera.Pos.x * camera.View._13 + camera.Pos.y * camera.View._23 + camera.Pos.z * camera.View._33);
		camera.View._14 = camera.View._24 = camera.View._34 = 0.0f;
		camera.View._44 = 1.0f;

	}

	void CreateProjectionMatrix(Camera& camera, float nearDist, float farDist, float fov, float aspectRatio)
	{
		camera.NearDistance = nearDist;
		camera.FarDistance = farDist;
		camera.FovY = fov;
		camera.AspectRatio = aspectRatio;

		float h = 1.0f / tan(fov * 0.5f);
		float w = h / camera.AspectRatio;
		float q = camera.FarDistance / (camera.FarDistance - camera.NearDistance);

		camera.Projection._11 = w;
		camera.Projection._22 = h;
		camera.Projection._33 = q;
		camera.Projection._43 = -q * camera.NearDistance;
		camera.Projection._34 = 1.0f;
		camera.Projection._44 = 0.0f;
	}

	void Update(Camera& camera)
	{
		camera.Right.x = camera.View._11;
		camera.Right.y = camera.View._21;
		camera.Right.z = camera.View._31;

		camera.Up.x = camera.View._12;
		camera.Up.y = camera.View._22;
		camera.Up.z = camera.View._32;

		camera.Look.x = camera.View._13;
		camera.Look.y = camera.View._23;
		camera.Look.z = camera.View._33;

		camera.Right.Normalize();
		camera.Up.Normalize();
		camera.Look.Normalize();
	}

	bool Frame(float dt)
	{
		return false;
	}

};