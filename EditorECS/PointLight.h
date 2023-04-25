#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLightComponent* LightComp;

public:
	PointLight();
	virtual ~PointLight();

public:
	bool SaveXML(std::wstring filename);
	bool LoadXML(std::wstring filename);

	virtual bool Save(std::string filename) override;
	virtual bool Load(std::string filename) override;
};