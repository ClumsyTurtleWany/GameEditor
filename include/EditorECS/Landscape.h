#pragma once
#include "Define.h"
#include "LandscapeComponents.h"
#include "TransformComponent.h"
#include "Entity.hpp"
#include "SculptingData.h"
#include "Camera.h"

class Landscape : public ECS::Entity
{
public:
	//std::vector<LandscapeComponent> Components;
	LandscapeComponents* Components;
	TransformComponent* Transform;
	
public:
	Landscape();
	virtual ~Landscape();
};
