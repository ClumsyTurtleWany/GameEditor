#pragma once
#include "Light.h"

class SpotLight : public Light
{
public:

public:
	SpotLight();
	virtual ~SpotLight();
};

inline SpotLight::SpotLight()
{
	this->AddComponent<SpotLightComponent>();
}

inline SpotLight::~SpotLight()
{

}