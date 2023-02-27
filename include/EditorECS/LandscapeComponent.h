#pragma once
#include "Define.h"
#include "MeshComponent.hpp"

struct LandscapeComponent : public MeshComponent
{
	int Row;
	int Column;
	DirectX::BoundingOrientedBox Box;
};