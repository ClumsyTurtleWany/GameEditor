#include "TimelineComponent.h"


TimelineComponent::TimelineComponent(float start, float goal,float time)
{
	From = start;
	To = goal;
	endTime = time;
}


void TimelineComponent::Update(float tick)
{
	
	float interpolateT;

	if (State == Playing && Result != nullptr)
	{		
		elapsedTime += (tick * Speed * Inverse);
		interpolateT = elapsedTime / endTime;
		// interpolate -> Result Update
		//interpolateT = max(0.000f, interpolateT);
		if (interpolateT < 0.f) interpolateT = 0.f;
		interpolateT = min(interpolateT, 1.0f);

		if (interpolateT == 0)
		{
			elapsedTime = 0.f;
			State = Start;
		}

		if (interpolateT == 1)
		{
			elapsedTime = endTime;
			State = End;
		}

		*Result = (1 - interpolateT) * From + interpolateT * To;
	}
	
	
}

void TimelineComponent::CreateTimeline(float start, float goal, float time)
{
	From = start;
	To = goal;
	endTime = time;
}

void TimelineComponent::SetTarget(float* result)
{
	if (result == nullptr)
		return;
	Result = result;
}


void TimelineComponent::SetSpeed(float speed)
{
	Speed = speed;
}

void TimelineComponent::SetInverse(bool setter)
{
	if (setter)
		Inverse = -1.f;
	else
		Inverse = 1.f;
}

void TimelineComponent::Play()
{
	State = Playing;
	SetInverse(false);
}

void TimelineComponent::PlayFromStart()
{
	State = Playing;
	SetInverse(false);
	elapsedTime = 0;
}

void TimelineComponent::Stop()
{
	State = Pause;
}

void TimelineComponent::Reverse()
{
	SetInverse(true);
}

void TimelineComponent::PlayFromEnd()
{
	State = Playing;
	SetInverse(true);
	elapsedTime = endTime;
}

float TimelineComponent::GetUpdate()
{
	return *Result;
}

TimelineState TimelineComponent::GetState()
{
	return State;
}
