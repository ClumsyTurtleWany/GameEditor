#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:

public:
	DirectionalLight();
	virtual ~DirectionalLight();
};

DirectionalLight::DirectionalLight()
{
	this->AddComponent<DirectionalLightComponent>();
}

DirectionalLight::~DirectionalLight()
{

}