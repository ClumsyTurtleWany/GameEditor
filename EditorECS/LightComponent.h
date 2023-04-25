#pragma once
#include "Define.h"
#include "LightData.h"

class DirectionalLightComponent : public ECS::Component
{
public:
	Vector4 Color;
	Vector4 Direction;

public:
	DirectionalLightComponent();
	~DirectionalLightComponent();

public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);
};

class PointLightComponent : public ECS::Component
{
public:
	Vector4 Color;
	Vector4 Direction;
	float Radius;

public:
	PointLightComponent();
	~PointLightComponent();

public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);
};

class SpotLightComponent : public ECS::Component
{
public:
	Vector4 Color;
	Vector4 Direction;
	float Radius;
	float Distance;

public:
	SpotLightComponent();
	~SpotLightComponent();

public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);
};