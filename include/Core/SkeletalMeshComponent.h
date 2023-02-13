#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "Material.h"
#include "MeshComponent.hpp"

class SkeletalMeshComponent
{
public:
	std::vector<MeshComponent> Meshes;

public:
	SkeletalMeshComponent() {};
};