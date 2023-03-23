#include "ColliderSystem.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"
#include "LandscapeComponents.h"

void ColliderSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<BoundingBoxComponent, TransformComponent>())
	{
		auto OBB = entity->GetComponent<BoundingBoxComponent>();
		auto Sphere = entity->GetComponent<BoundingSphereComponent>();
		auto transform = entity->GetComponent<TransformComponent>();

		if ((OBB != nullptr) && (transform != nullptr))
		{
			Matrix matWorld = Matrix::CreateScale(transform->Scale);
			matWorld *= Matrix::CreateFromYawPitchRoll(transform->Rotation);
			matWorld *= Matrix::CreateTranslation(transform->Translation);

			//In -> Out
			OBB->InitOBB.Transform(OBB->OBB, matWorld);

#ifdef _DEBUG
			if (OBB->pDebugObj)
			{
				OBB->pDebugObj->update(&matWorld, nullptr, nullptr);
			}
#endif // _DEBUG
		}

		if ((Sphere != nullptr) && (transform != nullptr))
		{
			Matrix matWorld = Matrix::CreateScale(transform->Scale);
			matWorld *= Matrix::CreateFromYawPitchRoll(transform->Rotation);
			matWorld *= Matrix::CreateTranslation(transform->Translation);

			Sphere->InitSphere.Transform(Sphere->Sphere, matWorld);

#ifdef _DEBUG
			if (Sphere->pDebugObj)
			{
				Sphere->pDebugObj->update(&matWorld, nullptr, nullptr);
			}
#endif // _DEBUG
		}
	}


//피킹 무빙 테스트용 코드
#ifdef _DEBUG
	for (auto& entity : world->GetEntities<LandscapeComponents, TransformComponent>())
	{
		auto landscape = entity->GetComponent<LandscapeComponents>();
		auto transform = entity->GetComponent<TransformComponent>();

		if (landscape != nullptr)
		{
			for (auto it : landscape->Components)
			{
				DirectX::BoundingBox temp = it.Box;
				temp.Center = temp.Center + transform->Translation;

				if (MAIN_PICKER.RayToAABB(temp))
				{
					for (auto& face : it.Faces)
					{
						Vector3 v0 = face.V0.Pos + transform->Translation;
						Vector3 v1 = face.V1.Pos + transform->Translation;
						Vector3 v2 = face.V2.Pos + transform->Translation;

						if (MAIN_PICKER.CheckPick(v0, v1, v2))
						{
							std::wstring Coord = L"MapPoint X : ";
							Coord += std::to_wstring(MAIN_PICKER.Intersection.x);
							Coord += L" Y : ";
							Coord += std::to_wstring(MAIN_PICKER.Intersection.y);
							Coord += L" Z : ";
							Coord += std::to_wstring(MAIN_PICKER.Intersection.z);
							Coord += L" \n";

							OutputDebugString(Coord.c_str());
							return;
						}
					}
				}
			}
		}
	}
#endif // _DEBUG
}
