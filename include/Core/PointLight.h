#pragma once
#include "Light.h"

class PointLight : public Light
{
public:

public:
	PointLight();
	virtual ~PointLight();
};

inline PointLight::PointLight()
{
	this->AddComponent<PointLightComponent>();
}

inline PointLight::~PointLight()
{

}