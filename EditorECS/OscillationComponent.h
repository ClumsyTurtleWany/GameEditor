#pragma once
#include "Define.h"
#include "TransformComponent.h"

enum class OSCILLATION_TYPE
{
	None,
	Damped,
	Harmonic,
};

class OscillationComponent
{
public:
	Vector3 ControlPoint;
	Vector3 Forward;
	Vector3 Up;
	float Frequency = 0.0f;
	float Duration = 0.0f;
	float ElapseTime = 0.0f;
	float Amplitude = 0.0f;
	float DampingCoefficient = 0.0f;
	OSCILLATION_TYPE OscillationType;
	bool isDone = true;

public:
	// pos : ���� ��ġ
	// forward : ����
	// amplitude : ����
	// duration : ���� �ð�
	// frep : ��
	// coef : ������. ���� ���� ������ ����.
	void Damped(Vector3 pos, Vector3 forward, float amplitude, float duration, float freq, float coef);
	void Update(Vector3& pos, float dt);
};