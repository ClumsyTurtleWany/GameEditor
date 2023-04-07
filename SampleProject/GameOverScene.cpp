#include "GameOverScene.h"

bool GameOverScene::Init()
{
	UI_Loader Loader;

	// Actor 생성
	Actor* actor = new Actor;
	auto tc = actor->AddComponent<WidgetComponent>();
	Loader.FileLoad(tc, L"../resource/UI/Save/GameOver.txt");

	// 메인 월드에 액터 추가.
	TheWorld.AddEntity(actor);

	// 시스템 추가.
	TheWorld.AddSystem(new WidgetRenderSystem);

	return true;
}
