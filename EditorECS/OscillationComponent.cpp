#include "OscillationComponent.h"
#include <cmath>

void OscillationComponent::Damped(Vector3 pos, Vector3 forward, float amplitude, float duration, float freq, float coef)
{
	if (!isDone)
	{
		return;
	}

	OscillationType = OSCILLATION_TYPE::Damped;
	ControlPoint = pos;
	Forward = forward;
	Amplitude = amplitude;
	Duration = duration;
	Frequency = freq;
	DampingCoefficient = coef;
	ElapseTime = 0.0f;
	isDone = false;

	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 right = ControlPoint.Cross(up);
	Up = right.Cross(Forward);
	Up.Normalize();
}

void OscillationComponent::Update(Vector3& pos, float dt)
{
	if (isDone)
	{
		return;
	}

	if (ElapseTime < Duration)
	{
		if (OscillationType == OSCILLATION_TYPE::Damped)
		{
			ElapseTime += dt;

			float s = sin((2.0f * PI) * Frequency * ElapseTime);
			float e = std::exp(-(DampingCoefficient * ElapseTime / 2.0f));
			float a = Amplitude * e * s;
			pos = ControlPoint + Up * a;

			if (ElapseTime > Duration)
			{
				pos = ControlPoint;
				OscillationType = OSCILLATION_TYPE::None;
				isDone = true;
			}
		}
	}
}