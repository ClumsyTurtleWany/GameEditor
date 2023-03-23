#include "BaseScene.h"

bool BaseScene::Init()
{
	return true;
}

bool BaseScene::Frame()
{
	return true;
}

bool BaseScene::Render()
{
	float dt = Timer::GetInstance()->SecondPerFrame;
	TheWorld.Tick(dt);
	return true;
}

bool BaseScene::Release()
{
	return true;
}