#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:

public:
	DirectionalLight();
	virtual ~DirectionalLight();
};

inline DirectionalLight::DirectionalLight()
{
	this->AddComponent<DirectionalLightComponent>();
}

inline DirectionalLight::~DirectionalLight()
{

}