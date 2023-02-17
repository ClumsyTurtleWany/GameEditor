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
	v.z = 1.0f;

	Matrix inversedView = DirectX::XMMatrixInverse(NULL, View);

	MouseRay.position = Vector3(0.0f, 0.0f, 0.0f);
	MouseRay.direction = Vector3(v.x, v.y, v.z);
	MouseRay.position = DirectX::XMVector3TransformCoord(MouseRay.position, inversedView);
	MouseRay.direction = DirectX::XMVector3TransformNormal(MouseRay.direction, inversedView);
	MouseRay.direction.Normalize();

}

bool MousePicker::CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
	float distance = 0.0f;
	if (IntersectTriangle(MouseRay.position, MouseRay.direction, v0, v1, v2, &distance))
	{
		IntercetionPosition = MouseRay.position + MouseRay.direction * distance;
		return true;
	}

	return false;
}

bool MousePicker::IntersectTriangle(const Vector3& origin, const Vector3& direction, const Vector3& v0, const Vector3& v1, const Vector3& v2, float* distance)
{
	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;

	Vector3 pvec = direction.Cross(edge2);
	float det = edge1.Dot(pvec);
	Vector3 tvec;

	if (det > 0)
	{
		tvec = origin - v0;
	}
	else
	{
		tvec = v0 - origin;
		det = -det;
	}

	if (det < 0.0001f)
	{
		return false;
	}

	float u = tvec.Dot(pvec);
	if (u < 0.0f || u > det)
	{
		return false;
	}

	Vector3 qvec = tvec.Cross(edge1);

	float v = direction.Dot(qvec);
	if (v < 0.0f || u + v > det)
	{
		return false;
	}

	*distance = edge2.Dot(qvec);
	float invDet = 1.0f / det;
	*distance *= invDet;
	u *= invDet;
	v *= invDet;

	return true;
}
