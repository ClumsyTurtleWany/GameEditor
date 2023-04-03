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
	SceneState SS = MAINTAIN; // 0�̸� ����, 1�̸� Ÿ��Ʋ��, 2�� ������, 3�̸� ������.. �� ���� �׷���?
	SceneState ID = MAINTAIN; // ������ �ڱⰡ ���� �ٿ���������

public:
	std::vector<DXTexture*> CardTextureList;
};

