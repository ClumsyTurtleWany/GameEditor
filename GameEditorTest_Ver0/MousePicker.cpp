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

	Vector3 v;
	v.x = (((2.0f * ptCursor.x) / ClientWidth) - 1) / Projection._11;
	v.y = -(((2.0f * ptCursor.y) / ClientHeight) - 1) / Projection._22;

	Matrix inversedView = DirectX::XMMatrixInverse(NULL, View);

	MouseRay.position = Vector3(0.0f, 0.0f, 0.0f);
	MouseRay.direction = Vector3(v.x, v.y, v.z);
	MouseRay.position = DirectX::XMVector3TransformCoord(MouseRay.position, inversedView);
	MouseRay.direction = DirectX::XMVector3TransformNormal(MouseRay.direction, inversedView);
	MouseRay.direction.Normalize();

}

bool MousePicker::CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
	return false;
}
