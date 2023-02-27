#include "Timer.hpp"

bool Timer::Initialize()
{
	GameTime = 0.0f;
	SecondPerFrame = 0.0f;
	Fps = 0;
	FpsCounter = 0;
	FpsTimer = 0.0f;

	BeforeTime = static_cast<float>(timeGetTime());

	// High Resolution Timer
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&Before); // == timeGetTime
	return true;
}

bool Timer::Frame()
{
	float currentTime = static_cast<float>(timeGetTime());
	SecondPerFrame = (currentTime - BeforeTime) / 1000.0f;
	GameTime += SecondPerFrame;
	FpsCounter++;
	BeforeTime = currentTime;
	
	// High Resolution Timer
	QueryPerformanceCounter(&Current);
	SecondPerFrame = (Current.QuadPart - Before.QuadPart) / static_cast<float>(Frequency.QuadPart);
	Before = Current;

	FpsTimer += SecondPerFrame;
	if (FpsTimer >= 1.0f)
	{
		Fps = static_cast<float>(FpsCounter);
		FpsTimer -= 1.0f;
		FpsCounter = 0;
	}
	return true;
}

bool Timer::Render()
{
	return true;
}

bool Timer::Release()
{
	return true;
}

float Timer::GetPlayTime()
{
	return GameTime;
}

float Timer::GetDeltaTime()
{
	return SecondPerFrame;
}

float Timer::GetFPS()
{
	return Fps;
}