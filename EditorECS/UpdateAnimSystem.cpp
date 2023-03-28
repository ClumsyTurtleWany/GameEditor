#include "UpdateAnimSystem.h"
#include "World.hpp"
#include "AnimationComponent.h"
#include "SkeletalMeshComponent.h"
#include "WeaponMeshComponent.h"

void UpdateAnimSystem::Tick(ECS::World* world, float time)
{
	for (auto& entity : world->GetEntities<SkeletalMeshComponent, AnimationComponent>())
	{
		auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
		auto animation = entity->GetComponent<AnimationComponent>();

		if ((skeletalMesh != nullptr) && (animation != nullptr))
		{
			animation->UpdateAnim(skeletalMesh, time);
		}
	}

	for (auto& entity : world->GetEntities<SkeletalMeshComponent, TransformComponent, WeaponMeshComponent>())
	{
		auto skeletalMesh = entity->GetComponent<SkeletalMeshComponent>();
		auto transform = entity->GetComponent<TransformComponent>();
		auto weaponMesh = entity->GetComponent<WeaponMeshComponent>();

		if ((skeletalMesh != nullptr) && (transform != nullptr) && (weaponMesh != nullptr))
		{
			int socketIdx = weaponMesh->SocketIndex;
			Matrix anim = skeletalMesh->BPAData.Bone[socketIdx];
			Matrix transform = skeletalMesh->TransformData.Mat;
			TransformMatrix pos;
			// anim &  transform은 이미 전치되어있으니까 행렬 곱의 전치연산 (드모르간?) 고려해서 역순으로 곱하기
			pos.Mat = transform * anim;
			//////// 전치 상태로 역행렬 구한다음에 다시 전치해주기
			pos.InversedMat = DirectX::XMMatrixInverse(0, pos.Mat);
			pos.InversedMat = pos.InversedMat.Transpose();

			weaponMesh->UpdatePos(pos);
		}
	}
}