#include "ColliderSystem.h"
#include "SkeletalMeshComponent.h"
#include "TransformComponent.h"
#include "BoundingBoxComponent.h"
#include "BoundingSphereComponent.h"
#include "LandscapeComponents.h"

void ColliderSystem::Tick(ECS::World* world, float time)
{
	MousePicker* pPicker = &MAIN_PICKER;
	pPicker->fLandTraceTimer += time;

	for (auto& entity : world->GetEntities<BoundingBoxComponent, TransformComponent>())
	{
		auto OBB = entity->GetComponent<BoundingBoxComponent>();
		auto Sphere = entity->GetComponent<BoundingSphereComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		auto skeletonMesh = entity->GetComponent<SkeletalMeshComponent>();

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

		if (pPicker->optPickingMode == PICKING_MODE::PMOD_CHARACTER && pPicker->bClicked)
		{
			if (skeletonMesh && pPicker->RayToOBB(OBB->OBB))
			{
				if (pPicker->fIntersectionDistance < pPicker->curSelect.fDistance)
				{
					pPicker->curSelect.pTarget = entity;
					pPicker->curSelect.vIntersection = pPicker->vIntersection;
					pPicker->curSelect.fDistance = pPicker->fIntersectionDistance;
				}
			}
		}
	}

	if (pPicker->optPickingMode == PICKING_MODE::PMOD_LAND && pPicker->bPendingClicked)
	{
		if (pPicker->fLandTraceTimer > PICKING_OP_TIME_LIMIT)
		{
			pPicker->bPendingClicked = false;
			pPicker->fLandTraceTimer = 0.0f;
			pPicker->curSelect.pTarget = nullptr;
			pPicker->curSelect.fDistance = MAX_PICK_DIST;

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

						if (pPicker->RayToAABB(temp))
						{
							for (auto& face : it.Faces)
							{
								Vector3 v0 = face.V0.Pos + transform->Translation;
								Vector3 v1 = face.V1.Pos + transform->Translation;
								Vector3 v2 = face.V2.Pos + transform->Translation;

								if (pPicker->CheckPick(v0, v1, v2))
								{
									pPicker->curSelect.pTarget = entity;
									pPicker->curSelect.vIntersection = pPicker->vIntersection;
									pPicker->curSelect.fDistance = pPicker->fIntersectionDistance;

#ifdef _DEBUG
									//좌표 출력 코드
									std::wstring Coord = L"MapPoint X : ";
									Coord += std::to_wstring(pPicker->vIntersection.x);
									Coord += L" Y : ";
									Coord += std::to_wstring(pPicker->vIntersection.y);
									Coord += L" Z : ";
									Coord += std::to_wstring(pPicker->vIntersection.z);
									Coord += L" \n";

									OutputDebugString(Coord.c_str());
#endif //_DEBUG

									return;
								}
							}
						}
					}
				}
			}
		}
	}

	if (pPicker->lastSelect.pTarget != pPicker->curSelect.pTarget)
	{
		memcpy(&(pPicker->lastSelect), &(pPicker->curSelect), sizeof(SelectState));
	}
}
