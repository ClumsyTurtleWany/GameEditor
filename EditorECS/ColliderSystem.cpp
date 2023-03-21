#include "ColliderSystem.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"

void ColliderSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<BoundingBoxComponent, TransformComponent>())
	{
		auto OBB = entity->GetComponent<BoundingBoxComponent>();
		auto Sphere = entity->GetComponent<BoundingSphereComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		if ((OBB != nullptr) && (transform != nullptr))
		{
			OBB->OBB.Center = OBB->InitOBB.Center + transform->Translation;
			OBB->OBB.Extents.x = OBB->InitOBB.Extents.x * transform->Scale.x;
			OBB->OBB.Extents.y = OBB->InitOBB.Extents.y * transform->Scale.y;
			OBB->OBB.Extents.z = OBB->InitOBB.Extents.z * transform->Scale.z;

			OBB->OBB.Orientation = Quaternion::CreateFromYawPitchRoll(transform->Rotation) * OBB->InitOBB.Orientation;
		}

		if ((Sphere != nullptr) && (transform != nullptr))
		{
			Sphere->Sphere.Center = Sphere->InitSphere.Center + transform->Translation;
			Sphere->Sphere.Radius = Sphere->InitSphere.Radius * 
				((transform->Scale.x + transform->Scale.y + transform->Scale.z) / 3.0f);
		}

		if (MAIN_PICKER.RayToOBB(OBB->OBB))
		{
			std::wstring Coord = L"OBB X : ";
			Coord += std::to_wstring(MAIN_PICKER.Intersection.x);
			Coord += L" Y : ";
			Coord += std::to_wstring(MAIN_PICKER.Intersection.y);
			Coord += L" Z : ";
			Coord += std::to_wstring(MAIN_PICKER.Intersection.z);
			Coord += L" \n";

			OutputDebugString(Coord.c_str());
		}

		if (MAIN_PICKER.RayToSphere(Sphere->Sphere))
		{
			std::wstring Coord = L"SPHERE X : ";
			Coord += std::to_wstring(MAIN_PICKER.Intersection.x);
			Coord += L" Y : ";
			Coord += std::to_wstring(MAIN_PICKER.Intersection.y);
			Coord += L" Z : ";
			Coord += std::to_wstring(MAIN_PICKER.Intersection.z);
			Coord += L" \n";

			OutputDebugString(Coord.c_str());
		}
	}
}
