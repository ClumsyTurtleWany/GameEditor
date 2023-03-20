#include "MousePicker.h"

MousePicker::MousePicker()
{
	isPickBVolume = false;
	isPickUI = false;
	pTarget = nullptr;
}

MousePicker::~MousePicker()
{
}

void MousePicker::Update()
{
	clearState();

	ptCursor = Input::GetInstance()->m_ptPos;
	ClientWidth = DXDevice::g_ViewPort.Width;
	ClientHeight = DXDevice::g_ViewPort.Height;

	Vector3 v;
	v.x = (((2.0f * ptCursor.x) / ClientWidth) - 1) / Projection._11;
	v.y = -(((2.0f * ptCursor.y) / ClientHeight) - 1) / Projection._22;
	v.z = 1.0f;

	Matrix inversedView = DirectX::XMMatrixInverse(NULL, View);

	PickingRay.position = Vector3(0.0f, 0.0f, 0.0f);
	PickingRay.direction = Vector3(v.x, v.y, v.z);
	PickingRay.position = DirectX::XMVector3TransformCoord(PickingRay.position, inversedView);
	PickingRay.direction = DirectX::XMVector3TransformNormal(PickingRay.direction, inversedView);
	PickingRay.direction.Normalize();
}

void MousePicker::clearState()
{
	isPickBVolume = false;
	isPickUI = false;

	pTarget = nullptr;
}

bool MousePicker::CheckPick(const Vector3& v0, const Vector3& v1, const Vector3& v2)
{
	float distance = 0.0f;
	if (IntersectTriangle(PickingRay.position, PickingRay.direction, v0, v1, v2, &distance))
	{
		IntercetionPosition = PickingRay.position + PickingRay.direction * distance;
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
