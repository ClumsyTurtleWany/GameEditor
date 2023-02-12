#pragma once
#include "Define.h"
#include "System.hpp"
#include "World.hpp"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"

class RenderSystem : public ECS::System
{
public:
	virtual void Tick(ECS::World* world, float time) override
	{
		for (auto& entity : world->GetEntities<SkeletalMeshComponent>())
		{
			auto skeletalMesh = entity.get()->GetComponent<SkeletalMeshComponent>();
			for (auto& it : skeletalMesh->Meshes)
			{
			
			}
			
		}

		for (auto& entity : world->GetEntities<StaticMeshComponent>())
		{
			auto staticMesh = entity.get()->GetComponent<StaticMeshComponent>();
			for (auto& it : staticMesh->Meshes)
			{

			}
		}
	}
};