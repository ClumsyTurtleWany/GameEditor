#pragma once
#include "Define.h"

class TransformComponent
{
public:
	Vector3 Translation;
	Vector3 Rotation;
	Vector3 Scale;

public:
	TransformComponent();
	TransformComponent(Vector3 translation, Vector3 rotation, Vector3 scale);
	
public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);
};