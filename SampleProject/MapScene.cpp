#include "MapScene.h"

bool MapScene::Init()
{
	ID = MAP;

	UI_Loader Loader;

	// Actor 생성
	Actor* MapUI = new Actor;
	//MapUI->Name = L"Map";
	auto mc = MapUI->AddComponent<WidgetComponent>();
	Loader.FileLoad(mc, L"../resource/UI/Save/Map.txt");
	BackButton = mc->FindObj(L"Back");
	Mon1 = mc->FindObj(L"Mon1");

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(MapUI);

	// 시스템 추가.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}

bool MapScene::Frame()
{
	if (Mon1->m_bClicked) 
	{
		Mon1->m_bClicked = false;
		SS = BATTLE;
	}

	BaseScene::Frame();
	return true;
}

bool MapScene::Render()
{
	BaseScene::Render();
	return true;
}

bool MapScene::Release()
{
	return true;
}
