#pragma once
#include "Define.h"
//#include "XmlParser.h"

class CameraArmComponent
{
public:
	float Distance = 0.0f;

	float Pitch = 0.0f;
	float Yaw = 0.0f;
	float Roll = 0.0f;
	
public:
	CameraArmComponent();
	virtual ~CameraArmComponent();

public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);
};