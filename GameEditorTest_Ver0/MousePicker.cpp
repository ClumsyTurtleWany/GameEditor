#include "pch.h"
#include "MousePicker.h"

MousePicker::MousePicker()
{
}

MousePicker::~MousePicker()
{
}

void MousePicker::Update()
{
	POINT ptCursor = Input::getInstance()->m_ptPos;

	Vector3 Vertex;
}
