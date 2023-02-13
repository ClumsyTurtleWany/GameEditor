#pragma once
#include "Entity.hpp"
#include "Mesh.hpp"
#include "TransformComponent.h"

class Landscape : public ECS::Entity
{
	Landscape();
	virtual ~Landscape();
};

Landscape::Landscape()
{
	this->AddComponent<StaticMesh>();
	this->AddComponent<TransformComponent>();
}

Landscape::~Landscape()
{

}