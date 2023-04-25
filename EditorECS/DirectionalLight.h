#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLightComponent* LightComp;

public:
	DirectionalLight();
	virtual ~DirectionalLight();

public:
	bool SaveXML(std::wstring filename);
	bool LoadXML(std::wstring filename);

	virtual bool Save(std::string filename) override;
	virtual bool Load(std::string filename) override;
};

