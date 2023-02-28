#pragma once
#include "Define.h"

// 교재 7강 Timer 참고할 것 나중에 HRT로 바꿀 것.
class Timer : public Singleton<Timer>
{
public:
	float	GameTime = 0.0f;
	float	SecondPerFrame = 0.0f;
	float	Fps = 0.0f;

private:
	float	BeforeTime = 0.0f;
	float	FpsTimer = 0.0f;
	UINT	FpsCounter = 0;

	// High Resolution Timer
public:
	LARGE_INTEGER Frequency;
	LARGE_INTEGER Current;
	LARGE_INTEGER Before;
	LARGE_INTEGER Elapse;

public:
	virtual bool Initialize();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();

public:
	float GetPlayTime();
	float GetDeltaTime();
	float GetFPS();

	
};

