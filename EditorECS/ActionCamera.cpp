#include "ActionCamera.h"

ActionCamera::ActionCamera()
{
	this->Name = L"ActionCamera"; // +std::to_wstring(this->ID);
	ActionCameraComp = this->AddComponent<ActionCameraComponent>();
	SplineComp = this->AddComponent<SplineComponent>();
	OscillationComp = this->AddComponent<OscillationComponent>();
	MovementComp = this->AddComponent<MovementComponent>();
	Arrow = this->AddComponent<ArrowComponent>();
}

ActionCamera::~ActionCamera()
{
}

void ActionCamera::LookAt(Vector3 target, float time)
{
	if (ActionCameraComp != nullptr)
	{
		if (!ActionCameraComp->isLockOn)
		{
			Vector3 targetDirection = target - ActionCameraComp->Pos;
			if (targetDirection.Length() < 0.1f)
			{
				return;
			}

			targetDirection.Normalize();
			float dist = ActionCameraComp->Look.Distance(ActionCameraComp->Look, targetDirection);
			if (dist < 0.1f)
			{
				return;
			}

			ActionCameraComp->PrevLook = ActionCameraComp->NextLook;
			ActionCameraComp->NextLook = target;
			ActionCameraComp->InterpolationTime = time;
			ActionCameraComp->ElapseTime = 0.0f;
			ActionCameraComp->isLockOn = true;
		}
	}
}

void ActionCamera::Damped(float amplitude, float duration, float freq, float coef)
{
	if (OscillationComp != nullptr)
	{
		OscillationComp->Damped(Transform->Translation, ActionCameraComp->Look, amplitude, duration, freq, coef);
	}
}

void ActionCamera::Lerp(Vector3 target1, Vector3 target2, float dist, float time)
{
	Vector3 center = (target1 + target2) / 2.0f;
	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 a = target1 - center;
	a.Normalize();
	Vector3 direction = a.Cross(up);
	direction.Normalize();
	Vector3 destination = center + direction * dist;

	MoveTo(destination, time);	
	LookAt(center, time);
}

void ActionCamera::MoveTo(Vector3 pos, float time)
{
	float dist = Transform->Translation.Distance(Transform->Translation, pos);
	Arrow->Forward = pos - Transform->Translation;
	Arrow->Forward.Normalize();
	Arrow->Right = (Arrow->Up.Cross(Arrow->Forward));
	Arrow->Right.Normalize();
	MovementComp->Speed = dist / time;
	MovementComp->Destination = pos;
}

void ActionCamera::Create(Vector3 eye, Vector3 target, float nearDist, float farDist, float fov, float aspectRatio)
{
	Arrow->Forward = target - eye;
	Arrow->Forward.Normalize();
	Arrow->Right = (Arrow->Up.Cross(Arrow->Forward));
	Arrow->Right.Normalize();

	//MoveTo(eye, 1.0f);
	Transform->Translation = eye;
	MovementComp->Destination = eye;

	ActionCameraComp->CreateViewMatrix(eye, target, Vector3(0.0f, 1.0f, 0.0f));
	ActionCameraComp->CreateProjectionMatrix(nearDist, farDist, fov, aspectRatio);
	LookAt(target, 1.0f);
}
