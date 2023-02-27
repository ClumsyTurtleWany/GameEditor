#pragma once
#include "Define.h"

// 교재 7강 Timer 참고할 것 나중에 HRT로 바꿀 것.
class Timer : public Singleton<Timer>
{
public:
	float	gameTime = 0.0f;
	float	secondPerFrame = 0.0f;
	float	fps = 0.0f;

private:
	float	beforeTime = 0.0f;
	float	fpsTimer = 0.0f;
	UINT	fpsCounter = 0;

	// High Resolution Timer
public:
	LARGE_INTEGER Frequency;
	LARGE_INTEGER Current;
	LARGE_INTEGER Before;
	LARGE_INTEGER Elapse;

public:
	virtual bool initialize();
	virtual bool frame();
	virtual bool render();
	virtual bool release();

public:
	float getPlayTime();
	float getDeltaTime();
	float getFPS();

	
};

