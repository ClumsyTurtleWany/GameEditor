#pragma once
#include "Define.hpp"

struct TransformComponent
{
	Vector3f Location;
	Vector3f Rotation;
	Vector3f Scale;

	TransformComponent() : Location({ 0.0, 0.0, 0.0}), Rotation({ 0.0, 0.0, 0.0}), Scale({ 1.0, 1.0, 1.0}) {};
	TransformComponent(Vector3f location, Vector3f rotation, Vector3f scale) : Location(location), Rotation(rotation), Scale(scale) {};
};