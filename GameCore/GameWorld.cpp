#include "GameWorld.hpp"

void GameWorld::SetMainCamera(Camera* camera)
{
	MainCamera = camera;
}

Camera* GameWorld::GetMainCamera()
{
	return MainCamera;
}
