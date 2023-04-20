#pragma once
#include "Actor.h"
#include "ActionCameraComponent.h"
#include "SplineComponent.h"
#include "OscillationComponent.h"
#include "MovementComponent.h"
#include "ArrowComponent.h"

class ActionCamera : public Actor
{
public:
	ActionCameraComponent* ActionCameraComp;
	SplineComponent* SplineComp;
	OscillationComponent* OscillationComp;
	MovementComponent* MovementComp;
	ArrowComponent* Arrow;

public:
	ActionCamera();
	virtual ~ActionCamera();

public:
	void LookAt(Vector3 target, float time);
	void Damped(float amplitude, float duration, float freq, float coef);
	void Lerp(Vector3 target1, Vector3 target2, float dist, float time);
	void MoveTo(Vector3 pos, float time);
	void Create(Vector3 eye, Vector3 target, float nearDist, float farDist, float fov, float aspectRatio);
};