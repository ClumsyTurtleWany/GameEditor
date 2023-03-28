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
			// anim &  transform�� �̹� ��ġ�Ǿ������ϱ� ��� ���� ��ġ���� (��𸣰�?) ����ؼ� �������� ���ϱ�
			pos.Mat = transform * anim;
			//////// ��ġ ���·� ����� ���Ѵ����� �ٽ� ��ġ���ֱ�
			pos.InversedMat = DirectX::XMMatrixInverse(0, pos.Mat);
			pos.InversedMat = pos.InversedMat.Transpose();

			weaponMesh->UpdatePos(pos);
		}
	}
}