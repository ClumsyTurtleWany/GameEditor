#include "ArrowComponent.h"

ArrowComponent::ArrowComponent()
{
	Forward		= Vector3::Forward;
	Up			= Vector3::Up;
	Right		= Vector3::Right;

	rotation = Quaternion::Identity;
}

ArrowComponent::~ArrowComponent()
{
}

void ArrowComponent::fromToArrow(Vector3 ToDir)
{
	Quaternion ToQ = Quaternion::FromToRotation(Forward, ToDir);
	rotation = ToQ * rotation;

	Forward = Vector3::Transform(Forward, ToQ);
	Up = Vector3::Transform(Up, ToQ);
	Right = Vector3::Transform(Right, ToQ);
}

void ArrowComponent::rotateArrow(Matrix& rot)
{
	Forward		= Vector3::Transform(Vector3::Forward,	rot);
	Up			= Vector3::Transform(Vector3::Up,		rot);
	Right		= Vector3::Transform(Vector3::Right,	rot);

	rotation = Quaternion::CreateFromRotationMatrix(rot);
}

void ArrowComponent::rotateArrow(Quaternion rot)
{
	Forward		= Vector3::Transform(Vector3::Forward,	rot);
	Up			= Vector3::Transform(Vector3::Up,		rot);
	Right		= Vector3::Transform(Vector3::Right,	rot);

	rotation = rot;
}

void ArrowComponent::rotateArrow(Vector3 angle)
{
	//angle = {Pitch, Yaw, Roll}
	Matrix rot = DirectX::XMMatrixRotationRollPitchYawFromVector(angle);

	Forward		= Vector3::Transform(Vector3::Forward,	rot);
	Up			= Vector3::Transform(Vector3::Up,		rot);
	Right		= Vector3::Transform(Vector3::Right,	rot);

	rotation = DirectX::XMQuaternionRotationRollPitchYawFromVector(angle);
}

Vector3 ArrowComponent::calculatePitchYawRollRadian(Quaternion q)
{
	Vector3 res;
	
	//2(yz+wx)
	float C0 = 2.0f * (q.y * q.z + q.x * q.w);

	//-x^2 -y^2 + z^2 + w^2
	float C1 = -1.0f * (q.x * q.x) -(q.y * q.y) + (q.z * q.z) + (q.w * q.w);

	//x^2 - y^2 - z^2 + w^2
	float C2 = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);

	//2(xy-zw)
	float C3 = 2.0f * (q.x * q.y - q.z * q.w);

	res.x = atan2f(C0, C1);
	res.y = asin(-C3);
	res.z = atan2f(C3, C2);

	return res;
}

Vector3 ArrowComponent::calculatePitchYawRollDegree(Quaternion q)
{
	Vector3 res = calculatePitchYawRollRadian(q) / PI * 180.0f;

	return res;
}
