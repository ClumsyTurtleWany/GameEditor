#include "ArrowComponent.h"

Vector3 calculatePitchYawRollRadian(Quaternion q)
{
	//좌표계 축에 대한 회전 연산순서에 따라 달리 결정된다.
	//DX의 PYR연산순서는 YXZ순서를 따르므로 이를 반영한다.
	//각 축의 180도 이상의 회전에 대해 조건처리를 하지 않았으므로 
	//회전의 방향이 항상 반시계 방향이 아닐 수 있음에 유의

	Vector3 vOut;

	//Yaw - Pitch - Roll
	float x = q.y;
	float y = q.x;
	float z = q.z;
	float w = q.w;

	float sqX = x * x;
	float sqY = y * y;
	float sqZ = z * z;
	float sqW = w * w;

	float C0 = 2.0f * (y * z + x * w);
	float C1 = -2.0f * (x * z - y * w);
	float C2 = 2.0f * (x * y + z * w);

	float C3 = -sqX - sqY + sqZ + sqW;
	float C4 = sqX - sqY - sqZ + sqW;

	vOut.x = atan2f(C0, C3);
	vOut.y = asinf(C1);
	vOut.z = atan2f(C2, C4);

	return vOut;
}

Vector3 calculatePitchYawRollDegree(Quaternion q)
{
	Vector3 vOut = calculatePitchYawRollRadian(q) / PI * 180.0f;

	return vOut;
}


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