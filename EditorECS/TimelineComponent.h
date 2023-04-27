#pragma once
#include "Define.h"

enum TimelineState
{
	Start,
	Playing,
	Pause,
	End,
};


class TimelineComponent
{
	float			From;
	float			To;
	float*			Result = nullptr;
	float			elapsedTime = 0.f;
	float			endTime = 0.f;
	TimelineState	State = Start;
	float			Speed = 1.f;
	float			Inverse = 1.f;

public:
	TimelineComponent() {};
	TimelineComponent(float start, float goal, float time);
	~TimelineComponent() {};

	
	void Update(float tick);
	void CreateTimeline(float start, float goal, float time);
	void SetTarget(float* result);
	void SetSpeed(float speed);
	void SetInverse(bool setter);
	void Play();
	void PlayFromStart();
	void Stop();
	void Reverse();
	void PlayFromEnd();

	float GetUpdate();
	TimelineState GetState();

};

