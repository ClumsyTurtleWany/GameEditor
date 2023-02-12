#pragma once
#include "Define.hpp"

struct MovementComponent
{
	Vector3f Location;
	Vector3f Forword;
	float	Speed;

	MovementComponent() : Location({ 0.0f, 0.0f, 0.0f }), Forword({ 0.0f, 0.0f, 0.0f }), Speed(0.0f) {};
	MovementComponent(Vector3f location, Vector3f forword, float speed) : Location(location), Forword(forword), Speed(speed) {};
};