#pragma once
#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLightComponent* LightComp;

public:
	SpotLight();
	virtual ~SpotLight();

public:
	bool SaveXML(std::wstring filename);
	bool LoadXML(std::wstring filename);

	virtual bool Save(std::string filename) override;
	virtual bool Load(std::string filename) override;
};