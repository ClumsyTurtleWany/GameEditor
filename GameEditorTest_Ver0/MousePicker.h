#pragma once
#include "Define.h"
#include "Input.hpp"

class MousePicker
{
public:
	Ray MouseRay;

public:
	MousePicker();
	~MousePicker();

public:
	void Update();


};