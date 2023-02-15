#pragma once
#include "Define.h"

class TransformComponent
{
public:
	Vector3 Translation;
	Vector3 Rotation;
	Vector3 Scale;

public:
	TransformComponent() : Translation(Vector3(0.0, 0.0, 0.0)), Rotation(Vector3(0.0, 0.0, 0.0)), Scale(Vector3(1.0, 1.0, 1.0)) {};
	TransformComponent(Vector3 translation, Vector3 rotation, Vector3 scale) : Translation(translation), Rotation(rotation), Scale(scale) {};
};