#pragma once
#include "World.hpp"
#include "Camera.h"

class GameWorld : public ECS::World
{
private:
	Camera* MainCamera = nullptr;

public:
	void SetMainCamera(Camera* camera);
	Camera* GetMainCamera();

};