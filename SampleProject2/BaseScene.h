#pragma once
#include "EditorCore.h"
#include "World.hpp"
#include "Camera.h"

#include "UI_Loader.h"
#include "WindowsClient.h"
#include "Actor.h"
#include "PlaneComponent.h"
#include "CameraSystem.h"
#include "RenderSystem.h"
#include "WidgetRenderSystem.h"
#include "Deck.h"

//server
#include "Host.h"
#include "Client.h"

enum SceneState
{
	MAINTAIN,
	LOADING,
	TITLE,
	MULTISELECT,
	MAP,
	BATTLE,
	MULTIBATTLE,
	DECKVIEW,
	REMAINVIEW,
	DISCARDVIEW,
	CLEAR,
	GAMEOVER,
};

class BaseScene
{
public:
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();

public:
	ECS::World TheWorld;
	Camera* MainCamera;
	//Camera* 

public:
	SceneState SS = MAINTAIN; // 0이면 유지, 1이면 타이틀로, 2면 맵으로, 3이면 전투로.. 뭐 대충 그런식?
	SceneState ID = MAINTAIN; // 씬마다 자기가 뭔지 붙여놓기위함

public:
	std::vector<DXTexture*> CardTextureList;
};

