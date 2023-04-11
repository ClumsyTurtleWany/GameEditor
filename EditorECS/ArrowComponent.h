#pragma once
#include "Define.h"

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

	static Vector3 calculatePitchYawRollRadian(Quaternion q);
	static Vector3 calculatePitchYawRollDegree(Quaternion q);
};

