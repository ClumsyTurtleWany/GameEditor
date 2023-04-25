#pragma once
#include "Define.h"

Vector3 calculatePitchYawRollRadian(Quaternion q);
Vector3 calculatePitchYawRollDegree(Quaternion q);

class ArrowComponent
{
public:
	Vector3 Forward;
	Vector3 Up;
	Vector3 Right;

	Quaternion rotation;

public:
	ArrowComponent();
	virtual ~ArrowComponent();

	void fromToArrow(Vector3 ToDir);
	void rotateArrow(Matrix& rot);
	void rotateArrow(Quaternion rot);
	void rotateArrow(Vector3 angle);

public:
	bool WriteXML(TiXmlElement* parent);
	bool ReadXML(TiXmlElement* parent);
};

