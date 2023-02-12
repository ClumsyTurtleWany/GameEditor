#pragma once
#include "Define.h"
#include "DXTexture.hpp"
#include "Material.h"
#include "MeshComponent.hpp"

class StaticMeshComponent
{
public:
	std::vector<MeshComponent> Meshes;

public:
	StaticMeshComponent() {};

public:
	bool Render();
};