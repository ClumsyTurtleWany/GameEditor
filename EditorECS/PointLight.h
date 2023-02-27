#pragma once
#include "Light.h"

class PointLight : public Light
{
public:

public:
	PointLight();
	virtual ~PointLight();
};

PointLight::PointLight()
{
	this->AddComponent<PointLightComponent>();
}

PointLight::~PointLight()
{

}